#!/usr/bin/env python3

from __future__ import annotations

import csv
import json
import math
import os
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
