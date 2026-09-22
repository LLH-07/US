#!/usr/bin/env python3

from __future__ import annotations

import argparse

from measurement_common import RESULTS_DIR
from scenario1_db_event import run_baseline, run_idle_polling
from scenario2_multi_session import run_multi_session
from scenario3_timer_expiry import run_timer_expiry


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
