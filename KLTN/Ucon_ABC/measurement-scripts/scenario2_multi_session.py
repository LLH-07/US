#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import random
import sys
import time
from datetime import datetime
from typing import Any, Sequence

import psycopg

from measurement_common import (
    RENTAL_ID,
    RESULTS_DIR,
    append_csv,
    create_active_sessions,
    ensure_no_active_measurement_sessions,
    ensure_no_pending_events,
    get_max_trigger_event_id,
    invalidate_rental,
    open_database,
    percentile_cont,
    prepare_rental,
    query_all,
    query_one,
    wait_until,
)


# ============================================================
# Scenario 2: one DB event affects many ACTIVE sessions
# ============================================================


MULTI_SESSION_SUMMARY_CSV_FIELDS = [
    "scenario",
    "poller_interval_ms",
    "requested_session_count",
    "iteration",
    "trigger_event_id",
    "affected_session_count",
    "revoked_session_count",
    "event_detection_latency_ms",
    "event_processing_latency_ms",
    "event_end_to_end_latency_ms",
    "reevaluation_total_ms",
    "reevaluation_mean_ms",
    "reevaluation_p95_ms",
    "reevaluation_max_ms",
    "revoke_mean_ms",
    "revoke_p95_ms",
    "revoke_max_ms",
    "success",
    "error",
]

MULTI_SESSION_DETAIL_CSV_FIELDS = [
    "scenario",
    "poller_interval_ms",
    "requested_session_count",
    "iteration",
    "trigger_event_id",
    "session_sequence",
    "session_id",
    "reevaluation_duration_ms",
    "revoke_latency_ms",
    "final_status",
]


def get_status_counts_for_sessions(
    connection: psycopg.Connection,
    session_ids: Sequence[int],
) -> dict[str, int]:
    rows = query_all(
        connection,
        """
        SELECT status, COUNT(*) AS status_count
        FROM usage_sessions
        WHERE id = ANY(%s::BIGINT[])
        GROUP BY status
        """,
        (list(session_ids),),
    )

    return {
        str(row["status"]): int(row["status_count"])
        for row in rows
    }


def wait_for_all_sessions_status(
    connection: psycopg.Connection,
    session_ids: Sequence[int],
    expected_status: str,
    timeout_seconds: float,
) -> dict[str, int]:
    expected_count = len(session_ids)

    return wait_until(
        operation=lambda: get_status_counts_for_sessions(
            connection,
            session_ids,
        ),
        predicate=lambda counts: (
            counts.get(expected_status, 0) == expected_count
        ),
        timeout_seconds=timeout_seconds,
        description=(
            f"all {expected_count} sessions to reach "
            f"status {expected_status}"
        ),
    )


def find_matching_db_event(
    connection: psycopg.Connection,
    minimum_trigger_event_id: int,
) -> dict[str, Any] | None:
    return query_one(
        connection,
        """
        SELECT
            id,
            occurred_at,
            picked_at,
            processed_at
        FROM ucon_trigger_events
        WHERE id > %s
          AND source_type = 'DB_TRIGGER'
          AND entity_type = 'USER_RENTALS'
          AND entity_id = %s
          AND attribute_name = 'status'
          AND event_type = 'ATTRIBUTE_CHANGED'
        ORDER BY id
        LIMIT 1
        """,
        (
            minimum_trigger_event_id,
            RENTAL_ID,
        ),
    )


def count_matching_db_events(
    connection: psycopg.Connection,
    minimum_trigger_event_id: int,
) -> int:
    row = query_one(
        connection,
        """
        SELECT COUNT(*) AS event_count
        FROM ucon_trigger_events
        WHERE id > %s
          AND source_type = 'DB_TRIGGER'
          AND entity_type = 'USER_RENTALS'
          AND entity_id = %s
          AND attribute_name = 'status'
          AND event_type = 'ATTRIBUTE_CHANGED'
        """,
        (
            minimum_trigger_event_id,
            RENTAL_ID,
        ),
    )

    return int(row["event_count"]) if row is not None else 0


def find_multi_session_details(
    connection: psycopg.Connection,
    trigger_event_id: int,
    session_ids: Sequence[int],
) -> list[dict[str, Any]]:
    return query_all(
        connection,
        """
        SELECT
            se.usage_session_id AS session_id,
            NULLIF(
                se.details_json ->> 'durationMs',
                ''
            )::DOUBLE PRECISION AS reevaluation_duration_ms,
            EXTRACT(
                EPOCH FROM (
                    us.revoked_at - te.occurred_at
                )
            ) * 1000.0 AS revoke_latency_ms,
            us.status AS final_status
        FROM session_events se
        JOIN ucon_trigger_events te
          ON te.id = se.trigger_event_id
        JOIN usage_sessions us
          ON us.id = se.usage_session_id
        WHERE se.trigger_event_id = %s
          AND se.event_type = 'EVENT_REEVALUATE'
          AND se.usage_session_id = ANY(%s::BIGINT[])
          AND us.revoked_at IS NOT NULL
        ORDER BY se.id
        """,
        (
            trigger_event_id,
            list(session_ids),
        ),
    )


def find_completed_multi_session_result(
    connection: psycopg.Connection,
    minimum_trigger_event_id: int,
    session_ids: Sequence[int],
) -> dict[str, Any] | None:
    event = find_matching_db_event(
        connection,
        minimum_trigger_event_id,
    )

    if event is None:
        return None

    if event["picked_at"] is None or event["processed_at"] is None:
        return None

    details = find_multi_session_details(
        connection,
        int(event["id"]),
        session_ids,
    )

    if len(details) != len(session_ids):
        return None

    if any(row["final_status"] != "REVOKED" for row in details):
        return None

    return {
        "event": event,
        "details": details,
    }


def wait_for_completed_multi_session_result(
    connection: psycopg.Connection,
    minimum_trigger_event_id: int,
    session_ids: Sequence[int],
    timeout_seconds: float,
) -> dict[str, Any]:
    return wait_until(
        operation=lambda: find_completed_multi_session_result(
            connection,
            minimum_trigger_event_id,
            session_ids,
        ),
        predicate=lambda result: result is not None,
        timeout_seconds=timeout_seconds,
        description=(
            f"completed DB-event metrics for "
            f"{len(session_ids)} sessions"
        ),
    )


def build_multi_session_metrics(
    result: dict[str, Any],
) -> tuple[dict[str, Any], list[dict[str, Any]]]:
    event = result["event"]
    details = result["details"]

    reevaluation_values = [
        float(row["reevaluation_duration_ms"])
        for row in details
    ]
    revoke_values = [
        float(row["revoke_latency_ms"])
        for row in details
    ]

    detection_ms = (
        event["picked_at"] - event["occurred_at"]
    ).total_seconds() * 1000.0
    processing_ms = (
        event["processed_at"] - event["picked_at"]
    ).total_seconds() * 1000.0
    end_to_end_ms = (
        event["processed_at"] - event["occurred_at"]
    ).total_seconds() * 1000.0

    summary = {
        "trigger_event_id": int(event["id"]),
        "affected_session_count": len(details),
        "revoked_session_count": sum(
            1 for row in details
            if row["final_status"] == "REVOKED"
        ),
        "event_detection_latency_ms": round(detection_ms, 3),
        "event_processing_latency_ms": round(processing_ms, 3),
        "event_end_to_end_latency_ms": round(end_to_end_ms, 3),
        "reevaluation_total_ms": round(
            sum(reevaluation_values),
            3,
        ),
        "reevaluation_mean_ms": round(
            sum(reevaluation_values) / len(reevaluation_values),
            3,
        ),
        "reevaluation_p95_ms": round(
            percentile_cont(reevaluation_values, 0.95),
            3,
        ),
        "reevaluation_max_ms": round(
            max(reevaluation_values),
            3,
        ),
        "revoke_mean_ms": round(
            sum(revoke_values) / len(revoke_values),
            3,
        ),
        "revoke_p95_ms": round(
            percentile_cont(revoke_values, 0.95),
            3,
        ),
        "revoke_max_ms": round(
            max(revoke_values),
            3,
        ),
    }

    detail_rows = [
        {
            "session_sequence": index,
            "session_id": int(row["session_id"]),
            "reevaluation_duration_ms": round(
                float(row["reevaluation_duration_ms"]),
                3,
            ),
            "revoke_latency_ms": round(
                float(row["revoke_latency_ms"]),
                3,
            ),
            "final_status": row["final_status"],
        }
        for index, row in enumerate(details, start=1)
    ]

    return summary, detail_rows


def run_single_multi_session_iteration(
    connection: psycopg.Connection,
    poller_interval_ms: int,
    session_count: int,
    pre_event_settle_seconds: float,
) -> tuple[dict[str, Any], list[dict[str, Any]]]:
    ensure_no_pending_events(connection)
    ensure_no_active_measurement_sessions(connection)
    prepare_rental(connection)

    session_ids = create_active_sessions(
        connection,
        session_count,
    )

    wait_for_all_sessions_status(
        connection=connection,
        session_ids=session_ids,
        expected_status="ACTIVE",
        timeout_seconds=max(15.0, session_count * 0.25 + 10.0),
    )

    if pre_event_settle_seconds > 0:
        time.sleep(pre_event_settle_seconds)

    trigger_marker = get_max_trigger_event_id(connection)

    random_delay_ms = random.uniform(
        0.0,
        float(poller_interval_ms),
    )
    time.sleep(random_delay_ms / 1000.0)

    invalidate_rental(connection)

    timeout_seconds = max(
        30.0,
        poller_interval_ms / 1000.0
        + 15.0
        + 0.5 * session_count,
    )

    wait_for_all_sessions_status(
        connection=connection,
        session_ids=session_ids,
        expected_status="REVOKED",
        timeout_seconds=timeout_seconds,
    )

    result = wait_for_completed_multi_session_result(
        connection=connection,
        minimum_trigger_event_id=trigger_marker,
        session_ids=session_ids,
        timeout_seconds=timeout_seconds,
    )

    matching_event_count = count_matching_db_events(
        connection,
        trigger_marker,
    )

    if matching_event_count != 1:
        raise RuntimeError(
            "Expected exactly one matching DB trigger event, "
            f"but found {matching_event_count}"
        )

    summary, details = build_multi_session_metrics(result)

    if summary["affected_session_count"] != session_count:
        raise RuntimeError(
            "Affected-session count does not match the request: "
            f"expected {session_count}, received "
            f"{summary['affected_session_count']}"
        )

    if summary["revoked_session_count"] != session_count:
        raise RuntimeError(
            "Not all requested sessions were revoked: "
            f"expected {session_count}, received "
            f"{summary['revoked_session_count']}"
        )

    return summary, details


def run_multi_session(args: argparse.Namespace) -> None:
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    summary_output_path = RESULTS_DIR / (
        "db_event_multi_session_summary_"
        f"{args.interval_ms}ms_"
        f"{args.session_count}sessions_"
        f"{timestamp}.csv"
    )
    detail_output_path = RESULTS_DIR / (
        "db_event_multi_session_details_"
        f"{args.interval_ms}ms_"
        f"{args.session_count}sessions_"
        f"{timestamp}.csv"
    )

    total_runs = args.warmup + args.iterations

    with open_database() as connection:
        for run_index in range(1, total_runs + 1):
            is_warmup = run_index <= args.warmup

            if is_warmup:
                display_iteration = run_index
                phase = "WARM-UP"
                phase_total = args.warmup
            else:
                display_iteration = run_index - args.warmup
                phase = "MEASURE"
                phase_total = args.iterations

            print(
                f"[{phase}] {display_iteration}/{phase_total}; "
                f"sessions={args.session_count}"
            )

            try:
                summary, details = run_single_multi_session_iteration(
                    connection=connection,
                    poller_interval_ms=args.interval_ms,
                    session_count=args.session_count,
                    pre_event_settle_seconds=(
                        args.pre_event_settle_seconds
                    ),
                )

                print(
                    json.dumps(
                        summary,
                        ensure_ascii=False,
                        indent=2,
                    )
                )

                if not is_warmup:
                    append_csv(
                        summary_output_path,
                        MULTI_SESSION_SUMMARY_CSV_FIELDS,
                        {
                            "scenario": "db_event_multi_session",
                            "poller_interval_ms": args.interval_ms,
                            "requested_session_count": (
                                args.session_count
                            ),
                            "iteration": display_iteration,
                            **summary,
                            "success": True,
                            "error": "",
                        },
                    )

                    for detail in details:
                        append_csv(
                            detail_output_path,
                            MULTI_SESSION_DETAIL_CSV_FIELDS,
                            {
                                "scenario": "db_event_multi_session",
                                "poller_interval_ms": (
                                    args.interval_ms
                                ),
                                "requested_session_count": (
                                    args.session_count
                                ),
                                "iteration": display_iteration,
                                "trigger_event_id": (
                                    summary["trigger_event_id"]
                                ),
                                **detail,
                            },
                        )

            except Exception as exc:
                if not is_warmup:
                    append_csv(
                        summary_output_path,
                        MULTI_SESSION_SUMMARY_CSV_FIELDS,
                        {
                            "scenario": "db_event_multi_session",
                            "poller_interval_ms": args.interval_ms,
                            "requested_session_count": (
                                args.session_count
                            ),
                            "iteration": display_iteration,
                            "success": False,
                            "error": str(exc),
                        },
                    )

                print(
                    f"Multi-session measurement failed: {exc}",
                    file=sys.stderr,
                )
                raise

    print(
        "\nSaved multi-session summary to:\n"
        f"{summary_output_path}"
    )
    print(
        "\nSaved per-session details to:\n"
        f"{detail_output_path}"
    )
