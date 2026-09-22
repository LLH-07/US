#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import random
import sys
from datetime import datetime
from typing import Any

import psycopg

from measurement_common import (
    RENTAL_ID,
    RESULTS_DIR,
    append_csv,
    create_active_session,
    ensure_no_active_measurement_sessions,
    ensure_no_pending_events,
    execute_returning_one,
    get_max_trigger_event_id,
    open_database,
    query_all,
    query_one,
    serialize_value,
    wait_for_session_status,
    wait_until,
)


# ============================================================
# Scenario 3: timer-based rental expiry
# ============================================================


TIMER_EXPIRY_CSV_FIELDS = [
    "scenario",
    "poller_interval_ms",
    "timer_scheduler_interval_ms",
    "expiry_lead_seconds",
    "random_phase_ms",
    "iteration",
    "session_id",
    "trigger_event_id",
    "dependency_due_at",
    "event_occurred_at",
    "event_picked_at",
    "event_processed_at",
    "session_revoked_at",
    "timer_detection_latency_ms",
    "timer_event_detection_latency_ms",
    "timer_event_processing_latency_ms",
    "timer_event_end_to_end_latency_ms",
    "reevaluation_duration_ms",
    "timer_total_revoke_latency_ms",
    "final_status",
    "success",
    "error",
]


def prepare_timer_rental(
    connection: psycopg.Connection,
    expiry_delay_seconds: float,
) -> datetime:
    """Prepare a short-lived rental only for scenario 3."""
    if expiry_delay_seconds <= 0:
        raise ValueError("expiry_delay_seconds must be positive")

    row = execute_returning_one(
        connection,
        """
        UPDATE user_rentals
        SET status = 'ACTIVE',
            expires_at = clock_timestamp()
                + (%s * INTERVAL '1 second')
        WHERE id = %s
        RETURNING expires_at
        """,
        (
            expiry_delay_seconds,
            RENTAL_ID,
        ),
    )

    if row is None:
        raise RuntimeError(
            f"Rental {RENTAL_ID} was not found"
        )

    return row["expires_at"]


def find_timer_dependency(
    connection: psycopg.Connection,
    session_id: int,
) -> dict[str, Any] | None:
    rows = query_all(
        connection,
        """
        SELECT
            usage_session_id,
            entity_type,
            entity_id,
            attribute_name,
            monitor_type,
            next_check_at
        FROM usage_session_dependencies
        WHERE usage_session_id = %s
          AND active_flag = true
          AND monitor_type IN ('TIMER', 'HYBRID')
          AND next_check_at IS NOT NULL
        ORDER BY next_check_at, entity_type, entity_id, attribute_name
        """,
        (session_id,),
    )

    if not rows:
        return None

    if len(rows) != 1:
        raise RuntimeError(
            f"Expected exactly one active TIMER/HYBRID dependency "
            f"for session {session_id}, but found {len(rows)}: {rows}"
        )

    return rows[0]


def wait_for_timer_dependency(
    connection: psycopg.Connection,
    session_id: int,
    timeout_seconds: float = 5.0,
) -> dict[str, Any]:
    return wait_until(
        operation=lambda: find_timer_dependency(
            connection,
            session_id,
        ),
        predicate=lambda row: row is not None,
        timeout_seconds=timeout_seconds,
        description=(
            f"active TIMER/HYBRID dependency for session {session_id}"
        ),
    )


def get_database_now(
    connection: psycopg.Connection,
) -> datetime:
    row = query_one(
        connection,
        """
        SELECT clock_timestamp()::timestamp AS database_now
        """,
    )

    if row is None:
        raise RuntimeError("Unable to read database clock")

    return row["database_now"]


def validate_timer_dependency(
    connection: psycopg.Connection,
    session_id: int,
    dependency: dict[str, Any],
    rental_expires_at: datetime,
) -> datetime:
    due_at = dependency["next_check_at"]

    if due_at is None:
        raise RuntimeError(
            f"Timer dependency for session {session_id} has no next_check_at"
        )

    if int(dependency["usage_session_id"]) != session_id:
        raise RuntimeError("Timer dependency belongs to another session")

    if int(dependency["entity_id"]) != RENTAL_ID:
        raise RuntimeError(
            "Timer dependency does not point to the measurement rental: "
            f"expected {RENTAL_ID}, received "
            f"{dependency['entity_id']}"
        )

    if str(dependency["attribute_name"]).lower() != "expires_at":
        raise RuntimeError(
            "Expected the timer dependency attribute to be expires_at, "
            f"received: {dependency['attribute_name']}"
        )

    difference_ms = abs(
        (due_at - rental_expires_at).total_seconds() * 1000.0
    )

    if difference_ms > 5.0:
        raise RuntimeError(
            "Timer dependency next_check_at does not match rental "
            f"expires_at (difference={difference_ms:.3f} ms)"
        )

    database_now = get_database_now(connection)
    remaining_ms = (due_at - database_now).total_seconds() * 1000.0

    if remaining_ms < 500.0:
        raise RuntimeError(
            "Timer dependency is too close to, or already past, its due "
            f"time (remaining={remaining_ms:.3f} ms). Increase "
            "--expiry-lead-seconds."
        )

    return due_at


def find_completed_timer_metrics(
    connection: psycopg.Connection,
    session_id: int,
    minimum_trigger_event_id: int,
    dependency_due_at: datetime,
) -> dict[str, Any] | None:
    return query_one(
        connection,
        """
        SELECT
            te.id AS trigger_event_id,
            se.usage_session_id AS session_id,
            %s::timestamp AS dependency_due_at,
            te.occurred_at AS event_occurred_at,
            te.picked_at AS event_picked_at,
            te.processed_at AS event_processed_at,
            us.revoked_at AS session_revoked_at,

            EXTRACT(
                EPOCH FROM (
                    te.occurred_at - %s::timestamp
                )
            ) * 1000.0
                AS timer_detection_latency_ms,

            EXTRACT(
                EPOCH FROM (
                    te.picked_at - te.occurred_at
                )
            ) * 1000.0
                AS timer_event_detection_latency_ms,

            EXTRACT(
                EPOCH FROM (
                    te.processed_at - te.picked_at
                )
            ) * 1000.0
                AS timer_event_processing_latency_ms,

            EXTRACT(
                EPOCH FROM (
                    te.processed_at - te.occurred_at
                )
            ) * 1000.0
                AS timer_event_end_to_end_latency_ms,

            NULLIF(
                se.details_json ->> 'durationMs',
                ''
            )::DOUBLE PRECISION
                AS reevaluation_duration_ms,

            EXTRACT(
                EPOCH FROM (
                    us.revoked_at - %s::timestamp
                )
            ) * 1000.0
                AS timer_total_revoke_latency_ms,

            us.status AS final_status

        FROM ucon_trigger_events te

        JOIN session_events se
          ON se.trigger_event_id = te.id
         AND se.usage_session_id = %s
         AND se.event_type = 'EVENT_REEVALUATE'

        JOIN usage_sessions us
          ON us.id = se.usage_session_id

        WHERE te.id > %s
          AND te.source_type = 'SCHEDULER'
          AND te.event_type = 'TIMER_DUE'
          AND NULLIF(
                te.payload_json ->> 'usage_session_id',
                ''
              )::BIGINT = %s
          AND te.picked_at IS NOT NULL
          AND te.processed_at IS NOT NULL
          AND us.revoked_at IS NOT NULL
          AND us.status = 'REVOKED'

        ORDER BY te.id
        LIMIT 1
        """,
        (
            dependency_due_at,
            dependency_due_at,
            dependency_due_at,
            session_id,
            minimum_trigger_event_id,
            session_id,
        ),
    )


def wait_for_completed_timer_metrics(
    connection: psycopg.Connection,
    session_id: int,
    minimum_trigger_event_id: int,
    dependency_due_at: datetime,
    timeout_seconds: float,
) -> dict[str, Any]:
    return wait_until(
        operation=lambda: find_completed_timer_metrics(
            connection=connection,
            session_id=session_id,
            minimum_trigger_event_id=minimum_trigger_event_id,
            dependency_due_at=dependency_due_at,
        ),
        predicate=lambda row: row is not None,
        timeout_seconds=timeout_seconds,
        description=(
            f"completed TIMER_DUE metrics for session {session_id}"
        ),
    )


def count_matching_timer_events(
    connection: psycopg.Connection,
    session_id: int,
    minimum_trigger_event_id: int,
) -> int:
    row = query_one(
        connection,
        """
        SELECT COUNT(*) AS event_count
        FROM ucon_trigger_events te
        WHERE te.id > %s
          AND te.source_type = 'SCHEDULER'
          AND te.event_type = 'TIMER_DUE'
          AND NULLIF(
                te.payload_json ->> 'usage_session_id',
                ''
              )::BIGINT = %s
        """,
        (
            minimum_trigger_event_id,
            session_id,
        ),
    )

    return int(row["event_count"]) if row is not None else 0


def validate_completed_timer_metrics(
    metrics: dict[str, Any],
) -> None:
    non_negative_metrics = (
        "timer_detection_latency_ms",
        "timer_event_detection_latency_ms",
        "timer_event_processing_latency_ms",
        "timer_event_end_to_end_latency_ms",
        "reevaluation_duration_ms",
        "timer_total_revoke_latency_ms",
    )

    for field in non_negative_metrics:
        value = metrics.get(field)

        if value is None:
            raise RuntimeError(f"Missing completed metric: {field}")

        if float(value) < 0:
            raise RuntimeError(
                f"Invalid negative metric {field}={value}"
            )

    if metrics.get("final_status") != "REVOKED":
        raise RuntimeError(
            "Timer iteration completed without a REVOKED session: "
            f"{metrics.get('final_status')}"
        )


def run_single_timer_iteration(
    connection: psycopg.Connection,
    poller_interval_ms: int,
    timer_scheduler_interval_ms: int,
    expiry_lead_seconds: float,
) -> dict[str, Any]:
    ensure_no_pending_events(connection)
    ensure_no_active_measurement_sessions(connection)

    random_phase_ms = random.uniform(
        0.0,
        float(timer_scheduler_interval_ms),
    )

    expiry_delay_seconds = (
        expiry_lead_seconds
        + random_phase_ms / 1000.0
    )

    rental_expires_at = prepare_timer_rental(
        connection=connection,
        expiry_delay_seconds=expiry_delay_seconds,
    )

    session_id = create_active_session(connection)

    dependency = wait_for_timer_dependency(
        connection=connection,
        session_id=session_id,
        timeout_seconds=5.0,
    )

    dependency_due_at = validate_timer_dependency(
        connection=connection,
        session_id=session_id,
        dependency=dependency,
        rental_expires_at=rental_expires_at,
    )

    trigger_marker = get_max_trigger_event_id(connection)

    timeout_seconds = max(
        30.0,
        expiry_lead_seconds
        + timer_scheduler_interval_ms / 1000.0
        + poller_interval_ms / 1000.0
        + 15.0,
    )

    wait_for_session_status(
        connection=connection,
        session_id=session_id,
        expected_status="REVOKED",
        timeout_seconds=timeout_seconds,
    )

    metrics = wait_for_completed_timer_metrics(
        connection=connection,
        session_id=session_id,
        minimum_trigger_event_id=trigger_marker,
        dependency_due_at=dependency_due_at,
        timeout_seconds=timeout_seconds,
    )

    matching_event_count = count_matching_timer_events(
        connection=connection,
        session_id=session_id,
        minimum_trigger_event_id=trigger_marker,
    )

    if matching_event_count != 1:
        raise RuntimeError(
            "Expected exactly one TIMER_DUE event for session "
            f"{session_id}, but found {matching_event_count}"
        )

    validate_completed_timer_metrics(metrics)

    return {
        "random_phase_ms": round(random_phase_ms, 3),
        **{
            key: serialize_value(value)
            for key, value in metrics.items()
        },
    }


def run_timer_expiry(args: argparse.Namespace) -> None:
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_path = RESULTS_DIR / (
        "timer_expiry_"
        f"poller{args.poller_interval_ms}ms_"
        f"scheduler{args.scheduler_interval_ms}ms_"
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
                f"poller={args.poller_interval_ms} ms; "
                f"scheduler={args.scheduler_interval_ms} ms"
            )

            try:
                metrics = run_single_timer_iteration(
                    connection=connection,
                    poller_interval_ms=args.poller_interval_ms,
                    timer_scheduler_interval_ms=(
                        args.scheduler_interval_ms
                    ),
                    expiry_lead_seconds=args.expiry_lead_seconds,
                )

                print(
                    json.dumps(
                        metrics,
                        ensure_ascii=False,
                        indent=2,
                        default=str,
                    )
                )

                if not is_warmup:
                    append_csv(
                        output_path,
                        TIMER_EXPIRY_CSV_FIELDS,
                        {
                            "scenario": "timer_expiry",
                            "poller_interval_ms": (
                                args.poller_interval_ms
                            ),
                            "timer_scheduler_interval_ms": (
                                args.scheduler_interval_ms
                            ),
                            "expiry_lead_seconds": (
                                args.expiry_lead_seconds
                            ),
                            "iteration": display_iteration,
                            **metrics,
                            "success": True,
                            "error": "",
                        },
                    )

            except Exception as exc:
                if not is_warmup:
                    append_csv(
                        output_path,
                        TIMER_EXPIRY_CSV_FIELDS,
                        {
                            "scenario": "timer_expiry",
                            "poller_interval_ms": (
                                args.poller_interval_ms
                            ),
                            "timer_scheduler_interval_ms": (
                                args.scheduler_interval_ms
                            ),
                            "expiry_lead_seconds": (
                                args.expiry_lead_seconds
                            ),
                            "iteration": display_iteration,
                            "success": False,
                            "error": str(exc),
                        },
                    )

                print(
                    f"Timer measurement failed: {exc}",
                    file=sys.stderr,
                )
                raise

    print(f"\nSaved timer-expiry results to:\n{output_path}")
