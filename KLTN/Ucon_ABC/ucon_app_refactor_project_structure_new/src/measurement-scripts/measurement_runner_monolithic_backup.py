#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import random
import sys
import time
from datetime import datetime
from decimal import Decimal
from pathlib import Path
from typing import Any, Callable, Sequence

import psycopg
import requests
from dotenv import load_dotenv
from psycopg.rows import dict_row


SCRIPT_DIR = Path(__file__).resolve().parent
RESULTS_DIR = SCRIPT_DIR / "results"

load_dotenv(SCRIPT_DIR / ".env")


def required_env(name: str) -> str:
    value = os.getenv(name)

    if value is None or value.strip() == "":
        raise RuntimeError(
            f"Missing required environment variable: {name}"
        )

    return value.strip()


def env_int(name: str) -> int:
    value = required_env(name)

    try:
        return int(value)
    except ValueError as exc:
        raise RuntimeError(
            f"{name} must be an integer, received: {value}"
        ) from exc


BASE_URL = required_env("UCON_BASE_URL").rstrip("/")
DB_DSN = required_env("UCON_DB_DSN")
DB_TIME_ZONE = os.getenv(
    "UCON_DB_TIME_ZONE",
    "Asia/Ho_Chi_Minh",
)

USER_ID = env_int("MEASUREMENT_USER_ID")
VOLUME_ID = env_int("MEASUREMENT_VOLUME_ID")
RENTAL_ID = env_int("MEASUREMENT_RENTAL_ID")
CHUNK_NO = env_int("MEASUREMENT_CHUNK_NO")

MAX_OBLIGATION_RETRIES = 5

TRY_ACCESS_PATH = os.getenv(
    "UCON_TRY_ACCESS_PATH",
    "/api/pep/read/try",
)

CHUNK_PATH_TEMPLATE = os.getenv(
    "UCON_CHUNK_PATH_TEMPLATE",
    "/api/pep/read/sessions/{session_id}/chunks/{chunk_no}",
)

REQUEST_TIMEOUT_SECONDS = 15
WAIT_POLL_SECONDS = 0.05


# ============================================================
# Shared database, HTTP, CSV and session helpers
# ============================================================


def open_database() -> psycopg.Connection:
    connection = psycopg.connect(
        DB_DSN,
        row_factory=dict_row,
    )

    with connection.cursor() as cursor:
        cursor.execute(
            """
            SELECT set_config(
                'TimeZone',
                %s,
                false
            )
            """,
            (DB_TIME_ZONE,),
        )

    connection.commit()
    return connection


def query_one(
    connection: psycopg.Connection,
    sql: str,
    parameters: tuple[Any, ...] = (),
) -> dict[str, Any] | None:
    with connection.cursor() as cursor:
        cursor.execute(sql, parameters)
        return cursor.fetchone()


def query_all(
    connection: psycopg.Connection,
    sql: str,
    parameters: tuple[Any, ...] = (),
) -> list[dict[str, Any]]:
    with connection.cursor() as cursor:
        cursor.execute(sql, parameters)
        return list(cursor.fetchall())


def execute_update(
    connection: psycopg.Connection,
    sql: str,
    parameters: tuple[Any, ...] = (),
) -> int:
    with connection.cursor() as cursor:
        cursor.execute(sql, parameters)
        affected_rows = cursor.rowcount

    connection.commit()
    return affected_rows


def execute_returning_one(
    connection: psycopg.Connection,
    sql: str,
    parameters: tuple[Any, ...] = (),
) -> dict[str, Any] | None:
    with connection.cursor() as cursor:
        cursor.execute(sql, parameters)
        row = cursor.fetchone()

    connection.commit()
    return row


def wait_until(
    operation: Callable[[], Any],
    predicate: Callable[[Any], bool],
    timeout_seconds: float,
    description: str,
) -> Any:
    deadline = time.monotonic() + timeout_seconds
    last_value: Any = None

    while time.monotonic() < deadline:
        last_value = operation()

        if predicate(last_value):
            return last_value

        time.sleep(WAIT_POLL_SECONDS)

    raise TimeoutError(
        f"Timed out waiting for {description}. "
        f"Last observed value: {last_value!r}"
    )


def round_metric(value: Any) -> Any:
    if isinstance(value, Decimal):
        return round(float(value), 3)

    if isinstance(value, float):
        return round(value, 3)

    return value


def serialize_value(value: Any) -> Any:
    if isinstance(value, datetime):
        return value.isoformat(timespec="microseconds")

    return round_metric(value)


def append_csv(
    output_path: Path,
    fieldnames: Sequence[str],
    row: dict[str, Any],
) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    file_exists = output_path.exists()

    unknown_fields = set(row) - set(fieldnames)
    if unknown_fields:
        raise RuntimeError(
            "CSV row contains unknown fields: "
            + ", ".join(sorted(unknown_fields))
        )

    with output_path.open(
        "a",
        newline="",
        encoding="utf-8",
    ) as file:
        writer = csv.DictWriter(
            file,
            fieldnames=fieldnames,
        )

        if not file_exists:
            writer.writeheader()

        writer.writerow(
            {
                field: serialize_value(row.get(field, ""))
                for field in fieldnames
            }
        )


def percentile_cont(
    values: Sequence[float],
    percentile: float,
) -> float:
    if not values:
        raise ValueError("Cannot calculate a percentile of an empty list")

    if percentile < 0.0 or percentile > 1.0:
        raise ValueError("percentile must be between 0 and 1")

    ordered = sorted(float(value) for value in values)

    if len(ordered) == 1:
        return ordered[0]

    position = (len(ordered) - 1) * percentile
    lower_index = math.floor(position)
    upper_index = math.ceil(position)

    if lower_index == upper_index:
        return ordered[lower_index]

    fraction = position - lower_index
    return (
        ordered[lower_index]
        + (ordered[upper_index] - ordered[lower_index]) * fraction
    )


def count_pending_events(
    connection: psycopg.Connection,
) -> int:
    row = query_one(
        connection,
        """
        SELECT COUNT(*) AS pending_count
        FROM ucon_trigger_events
        WHERE processed_flag = false
        """,
    )

    return int(row["pending_count"]) if row is not None else 0


def ensure_no_pending_events(
    connection: psycopg.Connection,
) -> None:
    pending_count = count_pending_events(connection)

    if pending_count != 0:
        raise RuntimeError(
            f"There are {pending_count} pending trigger event(s). "
            "Wait for the poller to finish or clean the measurement "
            "database before running the measurement."
        )


def find_active_measurement_sessions(
    connection: psycopg.Connection,
) -> list[int]:
    rows = query_all(
        connection,
        """
        SELECT id
        FROM usage_sessions
        WHERE subject_user_id = %s
          AND resource_type = 'BOOK_VOLUME'
          AND resource_id = %s
          AND action_id = 'READ'
          AND access_path = 'RENTAL'
          AND status = 'ACTIVE'
        ORDER BY id
        """,
        (
            USER_ID,
            VOLUME_ID,
        ),
    )

    return [int(row["id"]) for row in rows]


def ensure_no_active_measurement_sessions(
    connection: psycopg.Connection,
) -> None:
    session_ids = find_active_measurement_sessions(connection)

    if session_ids:
        raise RuntimeError(
            "Found stale ACTIVE measurement session(s): "
            f"{session_ids}. End/revoke them before rerunning "
            "the measurement."
        )


def prepare_rental(
    connection: psycopg.Connection,
) -> None:
    """Prepare a long-lived rental for DB-event scenarios 1 and 2."""
    affected_rows = execute_update(
        connection,
        """
        UPDATE user_rentals
        SET status = 'ACTIVE',
            expires_at = clock_timestamp() + INTERVAL '1 hour'
        WHERE id = %s
        """,
        (RENTAL_ID,),
    )

    if affected_rows != 1:
        raise RuntimeError(
            f"Rental {RENTAL_ID} was not found"
        )


def invalidate_rental(
    connection: psycopg.Connection,
) -> None:
    affected_rows = execute_update(
        connection,
        """
        UPDATE user_rentals
        SET status = 'CANCELLED'
        WHERE id = %s
          AND status = 'ACTIVE'
        """,
        (RENTAL_ID,),
    )

    if affected_rows != 1:
        raise RuntimeError(
            f"Rental {RENTAL_ID} was not ACTIVE "
            "when invalidation was requested"
        )


def find_session_id(value: Any) -> int | None:
    if isinstance(value, dict):
        possible_keys = (
            "sessionId",
            "session_id",
            "usageSessionId",
            "usage_session_id",
        )

        for key in possible_keys:
            candidate = value.get(key)

            if candidate is not None:
                return int(candidate)

        for nested_value in value.values():
            found = find_session_id(nested_value)

            if found is not None:
                return found

    if isinstance(value, list):
        for item in value:
            found = find_session_id(item)

            if found is not None:
                return found

    return None


def request_try_access() -> requests.Response:
    return requests.post(
        f"{BASE_URL}{TRY_ACCESS_PATH}",
        params={
            "userId": USER_ID,
            "volumeId": VOLUME_ID,
        },
        timeout=REQUEST_TIMEOUT_SECONDS,
    )


def require_success(
    response: requests.Response,
    operation: str,
) -> None:
    if response.ok:
        return

    try:
        response_body = json.dumps(
            response.json(),
            ensure_ascii=False,
        )
    except requests.JSONDecodeError:
        response_body = response.text

    raise RuntimeError(
        f"{operation} failed: "
        f"HTTP {response.status_code}; "
        f"response={response_body}"
    )


def extract_required_obligations(
    response_json: dict[str, Any],
) -> list[dict[str, Any]]:
    obligations: list[dict[str, Any]] = []
    attempts = response_json.get("attempts") or []

    for attempt in attempts:
        required = (
            attempt.get("requiredObligations")
            or attempt.get("required_obligations")
            or []
        )

        for obligation in required:
            item = dict(obligation)

            if not item.get("policyRef"):
                item["policyRef"] = attempt.get("policyRef")

            obligations.append(item)

    return obligations


def fulfill_obligation(
    obligation: dict[str, Any],
) -> None:
    obligation_id = obligation.get("obligationId")
    policy_ref = obligation.get("policyRef")
    obligation_type = obligation.get("type")

    if not obligation_id:
        raise RuntimeError(
            f"Missing obligationId: {obligation}"
        )

    if not policy_ref:
        raise RuntimeError(
            f"Missing policyRef: {obligation}"
        )

    if str(obligation_type).upper() != "CONSENT":
        raise RuntimeError(
            "Measurement runner currently supports only "
            f"CONSENT obligations, received: {obligation_type}"
        )

    response = requests.post(
        (
            f"{BASE_URL}/api/ucon/obligations/"
            f"{obligation_id}/fulfill"
        ),
        json={
            "userId": USER_ID,
            "policyRef": policy_ref,
            "evidence": {
                "accepted": True,
            },
        },
        timeout=REQUEST_TIMEOUT_SECONDS,
    )

    require_success(
        response,
        f"Fulfill obligation {obligation_id}",
    )


def get_session_status(
    connection: psycopg.Connection,
    session_id: int,
) -> str | None:
    row = query_one(
        connection,
        """
        SELECT status
        FROM usage_sessions
        WHERE id = %s
        """,
        (session_id,),
    )

    if row is None:
        return None

    return row["status"]


def wait_for_session_status(
    connection: psycopg.Connection,
    session_id: int,
    expected_status: str,
    timeout_seconds: float,
) -> str:
    return wait_until(
        operation=lambda: get_session_status(
            connection,
            session_id,
        ),
        predicate=lambda status: status == expected_status,
        timeout_seconds=timeout_seconds,
        description=(
            f"session {session_id} to reach "
            f"status {expected_status}"
        ),
    )


def create_active_session(
    connection: psycopg.Connection,
) -> int:
    response_json: dict[str, Any] | None = None
    session_id: int | None = None

    for attempt_number in range(
        1,
        MAX_OBLIGATION_RETRIES + 2,
    ):
        try_response = request_try_access()

        try:
            response_json = try_response.json()
        except requests.JSONDecodeError as exc:
            raise RuntimeError(
                "Try-access response was not valid JSON: "
                f"{try_response.text}"
            ) from exc

        session_id = find_session_id(response_json)

        if try_response.ok and session_id is not None:
            break

        reason_code = (
            response_json.get("reasonCode")
            or response_json.get("reason")
        )

        if reason_code != "PRE_OBLIGATION_REQUIRED":
            require_success(
                try_response,
                "Try access",
            )

            raise RuntimeError(
                "Try access returned no session ID: "
                f"{json.dumps(response_json, ensure_ascii=False)}"
            )

        required_obligations = extract_required_obligations(
            response_json
        )

        if not required_obligations:
            raise RuntimeError(
                "PRE_OBLIGATION_REQUIRED was returned, "
                "but response contains no requiredObligations. "
                "Check MeasurementSessionController response."
            )

        if attempt_number > MAX_OBLIGATION_RETRIES:
            raise RuntimeError(
                "Exceeded maximum obligation fulfillment retries"
            )

        print(
            f"  Fulfilling "
            f"{len(required_obligations)} obligation(s)..."
        )

        for obligation in required_obligations:
            fulfill_obligation(obligation)
    else:
        raise RuntimeError(
            "Unable to obtain a permitted session"
        )

    if session_id is None:
        raise RuntimeError(
            "Try access succeeded without session ID"
        )

    chunk_path = CHUNK_PATH_TEMPLATE.format(
        session_id=session_id,
        chunk_no=CHUNK_NO,
    )

    chunk_response = requests.get(
        f"{BASE_URL}{chunk_path}",
        timeout=REQUEST_TIMEOUT_SECONDS,
    )

    require_success(
        chunk_response,
        "Read first chunk",
    )

    wait_for_session_status(
        connection=connection,
        session_id=session_id,
        expected_status="ACTIVE",
        timeout_seconds=10,
    )

    return session_id


def create_active_sessions(
    connection: psycopg.Connection,
    session_count: int,
) -> list[int]:
    if session_count <= 0:
        raise ValueError("session_count must be positive")

    session_ids: list[int] = []
    progress_step = max(1, session_count // 10)

    for index in range(1, session_count + 1):
        session_ids.append(create_active_session(connection))

        if (
            index == 1
            or index == session_count
            or index % progress_step == 0
        ):
            print(
                f"  Active sessions: {index}/{session_count}"
            )

    if len(set(session_ids)) != session_count:
        raise RuntimeError(
            "Duplicate session IDs were returned while preparing "
            "the multi-session measurement"
        )

    return session_ids


def get_max_trigger_event_id(
    connection: psycopg.Connection,
) -> int:
    row = query_one(
        connection,
        """
        SELECT COALESCE(MAX(id), 0) AS max_id
        FROM ucon_trigger_events
        """,
    )

    return int(row["max_id"]) if row is not None else 0


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
        SELECT clock_timestamp() AS database_now
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


# ============================================================
# Command line interface
# ============================================================


def positive_int(value: str) -> int:
    parsed = int(value)
    if parsed <= 0:
        raise argparse.ArgumentTypeError("must be positive")
    return parsed


def non_negative_int(value: str) -> int:
    parsed = int(value)
    if parsed < 0:
        raise argparse.ArgumentTypeError("must be zero or positive")
    return parsed


def positive_float(value: str) -> float:
    parsed = float(value)
    if parsed <= 0:
        raise argparse.ArgumentTypeError("must be positive")
    return parsed


def non_negative_float(value: str) -> float:
    parsed = float(value)
    if parsed < 0:
        raise argparse.ArgumentTypeError("must be zero or positive")
    return parsed


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="UCON measurement automation runner"
    )

    subparsers = parser.add_subparsers(
        dest="scenario",
        required=True,
    )

    baseline_parser = subparsers.add_parser(
        "baseline",
        help="Run one-session one-DB-event measurements",
    )
    baseline_parser.add_argument(
        "--interval-ms",
        type=positive_int,
        required=True,
        help=(
            "Poller interval currently configured "
            "in the running Backend"
        ),
    )
    baseline_parser.add_argument(
        "--warmup",
        type=non_negative_int,
        default=10,
    )
    baseline_parser.add_argument(
        "--iterations",
        type=positive_int,
        default=50,
    )

    multi_parser = subparsers.add_parser(
        "multi-session",
        help="Run one DB event affecting many ACTIVE sessions",
    )
    multi_parser.add_argument(
        "--interval-ms",
        type=positive_int,
        required=True,
    )
    multi_parser.add_argument(
        "--session-count",
        type=positive_int,
        required=True,
    )
    multi_parser.add_argument(
        "--pre-event-settle-seconds",
        type=non_negative_float,
        default=1.0,
    )
    multi_parser.add_argument(
        "--warmup",
        type=non_negative_int,
        default=10,
    )
    multi_parser.add_argument(
        "--iterations",
        type=positive_int,
        default=50,
    )

    idle_parser = subparsers.add_parser(
        "idle",
        help="Measure poller activity without events",
    )
    idle_parser.add_argument(
        "--interval-ms",
        type=positive_int,
        required=True,
    )
    idle_parser.add_argument(
        "--duration-seconds",
        type=positive_int,
        default=300,
    )

    timer_parser = subparsers.add_parser(
        "timer-expiry",
        help="Measure timer-based rental-expiry revocation",
    )
    timer_parser.add_argument(
        "--poller-interval-ms",
        type=positive_int,
        required=True,
        help=(
            "ucon.trigger-poller.interval-ms configured in the "
            "running Backend"
        ),
    )
    timer_parser.add_argument(
        "--scheduler-interval-ms",
        type=positive_int,
        required=True,
        help=(
            "ucon.timer-scheduler.interval-ms configured in the "
            "running Backend"
        ),
    )
    timer_parser.add_argument(
        "--expiry-lead-seconds",
        type=positive_float,
        default=3.0,
        help=(
            "Minimum time from rental preparation to expires_at. "
            "A random phase in [0, scheduler interval) is added."
        ),
    )
    timer_parser.add_argument(
        "--warmup",
        type=non_negative_int,
        default=10,
    )
    timer_parser.add_argument(
        "--iterations",
        type=positive_int,
        default=50,
    )

    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()

    RESULTS_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    if args.scenario == "baseline":
        run_baseline(args)
        return

    if args.scenario == "multi-session":
        run_multi_session(args)
        return

    if args.scenario == "idle":
        run_idle_polling(args)
        return

    if args.scenario == "timer-expiry":
        run_timer_expiry(args)
        return

    parser.error(
        f"Unsupported scenario: {args.scenario}"
    )


if __name__ == "__main__":
    main()