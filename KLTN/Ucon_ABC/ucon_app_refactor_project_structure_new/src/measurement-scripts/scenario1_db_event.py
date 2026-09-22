#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import random
import sys
import time
from datetime import datetime
from typing import Any

import psycopg
import requests

from measurement_common import (
    BASE_URL,
    REQUEST_TIMEOUT_SECONDS,
    RESULTS_DIR,
    append_csv,
    create_active_session,
    ensure_no_pending_events,
    get_max_trigger_event_id,
    invalidate_rental,
    open_database,
    prepare_rental,
    query_one,
    round_metric,
    wait_for_session_status,
    wait_until,
)


# ============================================================
# Scenario 1: one ACTIVE session and one DB event
# ============================================================


BASELINE_CSV_FIELDS = [
    "scenario",
    "poller_interval_ms",
    "iteration",
    "session_id",
    "trigger_event_id",
    "event_detection_latency_ms",
    "event_processing_latency_ms",
    "event_end_to_end_latency_ms",
    "reevaluation_duration_ms",
    "revoke_latency_ms",
    "success",
    "error",
]


def find_completed_metrics(
    connection: psycopg.Connection,
    session_id: int,
    minimum_trigger_event_id: int,
) -> dict[str, Any] | None:
    return query_one(
        connection,
        """
        SELECT
            te.id AS trigger_event_id,
            se.usage_session_id AS session_id,

            EXTRACT(
                EPOCH FROM (
                    te.picked_at - te.occurred_at
                )
            ) * 1000.0
                AS event_detection_latency_ms,

            EXTRACT(
                EPOCH FROM (
                    te.processed_at - te.picked_at
                )
            ) * 1000.0
                AS event_processing_latency_ms,

            EXTRACT(
                EPOCH FROM (
                    te.processed_at - te.occurred_at
                )
            ) * 1000.0
                AS event_end_to_end_latency_ms,

            NULLIF(
                se.details_json ->> 'durationMs',
                ''
            )::DOUBLE PRECISION
                AS reevaluation_duration_ms,

            EXTRACT(
                EPOCH FROM (
                    us.revoked_at - te.occurred_at
                )
            ) * 1000.0
                AS revoke_latency_ms

        FROM session_events se

        JOIN ucon_trigger_events te
          ON te.id = se.trigger_event_id

        JOIN usage_sessions us
          ON us.id = se.usage_session_id

        WHERE se.usage_session_id = %s
          AND se.event_type = 'EVENT_REEVALUATE'
          AND te.id > %s
          AND te.picked_at IS NOT NULL
          AND te.processed_at IS NOT NULL
          AND us.revoked_at IS NOT NULL

        ORDER BY te.id DESC
        LIMIT 1
        """,
        (
            session_id,
            minimum_trigger_event_id,
        ),
    )


def wait_for_metrics(
    connection: psycopg.Connection,
    session_id: int,
    minimum_trigger_event_id: int,
    timeout_seconds: float,
) -> dict[str, Any]:
    return wait_until(
        operation=lambda: find_completed_metrics(
            connection,
            session_id,
            minimum_trigger_event_id,
        ),
        predicate=lambda row: row is not None,
        timeout_seconds=timeout_seconds,
        description=(
            f"completed metrics for session {session_id}"
        ),
    )


def run_single_db_event_iteration(
    connection: psycopg.Connection,
    poller_interval_ms: int,
) -> dict[str, Any]:
    prepare_rental(connection)
    session_id = create_active_session(connection)
    trigger_marker = get_max_trigger_event_id(connection)

    random_delay_ms = random.uniform(
        0,
        poller_interval_ms,
    )
    time.sleep(random_delay_ms / 1000.0)

    invalidate_rental(connection)

    timeout_seconds = max(
        15.0,
        poller_interval_ms / 1000.0 + 10.0,
    )

    wait_for_session_status(
        connection=connection,
        session_id=session_id,
        expected_status="REVOKED",
        timeout_seconds=timeout_seconds,
    )

    metrics = wait_for_metrics(
        connection=connection,
        session_id=session_id,
        minimum_trigger_event_id=trigger_marker,
        timeout_seconds=timeout_seconds,
    )

    return {
        key: round_metric(value)
        for key, value in metrics.items()
    }


def run_baseline(args: argparse.Namespace) -> None:
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_path = RESULTS_DIR / (
        f"db_event_baseline_"
        f"{args.interval_ms}ms_"
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
                f"[{phase}] "
                f"{display_iteration}/{phase_total}"
            )

            try:
                metrics = run_single_db_event_iteration(
                    connection=connection,
                    poller_interval_ms=args.interval_ms,
                )

                print(
                    json.dumps(
                        metrics,
                        ensure_ascii=False,
                        indent=2,
                    )
                )

                if not is_warmup:
                    append_csv(
                        output_path,
                        BASELINE_CSV_FIELDS,
                        {
                            "scenario": "db_event_baseline",
                            "poller_interval_ms": args.interval_ms,
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
                        BASELINE_CSV_FIELDS,
                        {
                            "scenario": "db_event_baseline",
                            "poller_interval_ms": args.interval_ms,
                            "iteration": display_iteration,
                            "success": False,
                            "error": str(exc),
                        },
                    )

                print(
                    f"Measurement failed: {exc}",
                    file=sys.stderr,
                )
                raise

    print(f"\nSaved results to:\n{output_path}")


# ============================================================
# Idle polling measurement
# ============================================================


def run_idle_polling(args: argparse.Namespace) -> None:
    with open_database() as connection:
        ensure_no_pending_events(connection)

    reset_response = requests.post(
        f"{BASE_URL}/api/measurement/poller/reset",
        timeout=REQUEST_TIMEOUT_SECONDS,
    )
    reset_response.raise_for_status()

    print(
        f"Collecting idle polling metrics for "
        f"{args.duration_seconds} seconds..."
    )

    time.sleep(args.duration_seconds)

    snapshot_response = requests.get(
        f"{BASE_URL}/api/measurement/poller/snapshot",
        timeout=REQUEST_TIMEOUT_SECONDS,
    )
    snapshot_response.raise_for_status()

    snapshot = snapshot_response.json()
    snapshot["scenario"] = "idle_polling"
    snapshot["pollerIntervalMs"] = args.interval_ms

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_path = RESULTS_DIR / (
        f"idle_polling_"
        f"{args.interval_ms}ms_"
        f"{timestamp}.json"
    )

    output_path.write_text(
        json.dumps(
            snapshot,
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )

    print(
        json.dumps(
            snapshot,
            ensure_ascii=False,
            indent=2,
        )
    )

    print(f"\nSaved results to:\n{output_path}")
