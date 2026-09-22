#!/usr/bin/env bash
set -Eeuo pipefail

# ============================================================
# UCON scenario 2 automation: one DB event affects N sessions.
#
# Default configuration:
#   - Poller interval fixed at 1000 ms.
#   - Session counts: 1, 10, 50, 100.
#   - A separate Backend process is started for each count.
#   - Batch 1: 30 warm-up + 50 measured iterations.
#   - Batch 2: 10 warm-up + 50 measured iterations.
#
# Override examples:
#   SESSION_COUNTS="1 10" ./run_db_event_multi_session.sh
#   BATCH1_WARMUP=2 BATCH1_ITERATIONS=3 \
#   BATCH2_WARMUP=1 BATCH2_ITERATIONS=3 \
#   ./run_db_event_multi_session.sh
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BE_DIR="${SCRIPT_DIR}/../BE"
RUNNER="${SCRIPT_DIR}/measurement_runner.py"
ENV_FILE="${SCRIPT_DIR}/.env"
PYTHON_BIN="${SCRIPT_DIR}/.venv/bin/python"

RESULTS_DIR="${SCRIPT_DIR}/results"
AUTOMATION_LOG_DIR="${RESULTS_DIR}/automation_logs"

POLLER_INTERVAL_MS="${POLLER_INTERVAL_MS:-1000}"
SESSION_COUNTS_STRING="${SESSION_COUNTS:-1 10 50 100}"

BATCH1_WARMUP="${BATCH1_WARMUP:-30}"
BATCH1_ITERATIONS="${BATCH1_ITERATIONS:-50}"
BATCH2_WARMUP="${BATCH2_WARMUP:-10}"
BATCH2_ITERATIONS="${BATCH2_ITERATIONS:-50}"

STABILIZATION_SECONDS="${STABILIZATION_SECONDS:-15}"
PRE_EVENT_SETTLE_SECONDS="${PRE_EVENT_SETTLE_SECONDS:-1}"
BETWEEN_BATCHES_SECONDS="${BETWEEN_BATCHES_SECONDS:-5}"
BETWEEN_COUNTS_SECONDS="${BETWEEN_COUNTS_SECONDS:-5}"
BACKEND_START_TIMEOUT_SECONDS="${BACKEND_START_TIMEOUT_SECONDS:-120}"

read -r -a SESSION_COUNTS <<< "${SESSION_COUNTS_STRING}"

BE_PID=""
BE_LOG=""
MAVEN_CMD=""
BASE_URL=""


timestamp() {
    date +"%Y-%m-%d %H:%M:%S"
}


log() {
    printf '[%s] %s\n' "$(timestamp)" "$*"
}


die() {
    printf '[%s] ERROR: %s\n' "$(timestamp)" "$*" >&2
    exit 1
}


require_file() {
    local path="$1"
    [[ -f "${path}" ]] || die "Không tìm thấy file: ${path}"
}


require_directory() {
    local path="$1"
    [[ -d "${path}" ]] || die "Không tìm thấy thư mục: ${path}"
}


check_non_negative_integer() {
    local name="$1"
    local value="$2"

    [[ "${value}" =~ ^[0-9]+$ ]] \
        || die "${name} phải là số nguyên không âm, nhận được: ${value}"
}


check_positive_integer() {
    local name="$1"
    local value="$2"

    [[ "${value}" =~ ^[1-9][0-9]*$ ]] \
        || die "${name} phải là số nguyên dương, nhận được: ${value}"
}


load_base_url() {
    local value

    value="$(
        grep -E '^[[:space:]]*UCON_BASE_URL=' "${ENV_FILE}" \
            | tail -n 1 \
            | cut -d '=' -f 2- \
            | tr -d '\r' \
            | xargs
    )"

    if [[ -z "${value}" ]]; then
        value="http://localhost:8081"
    fi

    printf '%s' "${value%/}"
}


check_prerequisites() {
    require_directory "${BE_DIR}"
    require_file "${BE_DIR}/pom.xml"
    require_file "${RUNNER}"
    require_file "${ENV_FILE}"
    require_file "${PYTHON_BIN}"

    command -v curl >/dev/null 2>&1 \
        || die "Không tìm thấy curl trong PATH."

    if [[ -x "${BE_DIR}/mvnw" ]]; then
        MAVEN_CMD="${BE_DIR}/mvnw"
    else
        MAVEN_CMD="$(command -v mvn || true)"
        [[ -n "${MAVEN_CMD}" ]] \
            || die "Không tìm thấy mvn và Backend cũng không có mvnw."
    fi

    check_positive_integer \
        "POLLER_INTERVAL_MS" \
        "${POLLER_INTERVAL_MS}"
    check_non_negative_integer \
        "BATCH1_WARMUP" \
        "${BATCH1_WARMUP}"
    check_positive_integer \
        "BATCH1_ITERATIONS" \
        "${BATCH1_ITERATIONS}"
    check_non_negative_integer \
        "BATCH2_WARMUP" \
        "${BATCH2_WARMUP}"
    check_positive_integer \
        "BATCH2_ITERATIONS" \
        "${BATCH2_ITERATIONS}"
    check_positive_integer \
        "STABILIZATION_SECONDS" \
        "${STABILIZATION_SECONDS}"

    local session_count
    for session_count in "${SESSION_COUNTS[@]}"; do
        check_positive_integer \
            "SESSION_COUNTS item" \
            "${session_count}"
    done

    mkdir -p "${AUTOMATION_LOG_DIR}"
}


show_backend_log_tail() {
    if [[ -n "${BE_LOG}" && -f "${BE_LOG}" ]]; then
        printf '\n----- 100 dòng cuối Backend log -----\n' >&2
        tail -n 100 "${BE_LOG}" >&2 || true
        printf '%s\n\n' '------------------------------------' >&2
    fi
}


stop_backend() {
    if [[ -z "${BE_PID}" ]]; then
        return
    fi

    if kill -0 "${BE_PID}" 2>/dev/null; then
        log "Đang dừng Backend PID ${BE_PID}..."
        kill -TERM "${BE_PID}" 2>/dev/null || true

        local waited=0
        while kill -0 "${BE_PID}" 2>/dev/null \
            && [[ "${waited}" -lt 30 ]]; do
            sleep 1
            waited=$((waited + 1))
        done

        pkill -TERM -P "${BE_PID}" 2>/dev/null || true

        if kill -0 "${BE_PID}" 2>/dev/null; then
            log "Backend chưa dừng sau 30 giây; buộc dừng."
            kill -KILL "${BE_PID}" 2>/dev/null || true
        fi

        wait "${BE_PID}" 2>/dev/null || true
    fi

    BE_PID=""
}


cleanup() {
    local exit_code=$?

    stop_backend

    if [[ "${exit_code}" -ne 0 ]]; then
        show_backend_log_tail
        log "Quy trình Kịch bản 2 dừng do lỗi."
    fi

    exit "${exit_code}"
}


trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM


ensure_no_existing_backend() {
    if curl -fsS \
        --max-time 2 \
        "${BASE_URL}/api/measurement/poller/snapshot" \
        >/dev/null 2>&1; then
        die "Đã có Backend measurement chạy tại ${BASE_URL}. Hãy dừng nó trước."
    fi
}


start_backend() {
    local session_count="$1"
    local run_stamp

    run_stamp="$(date +"%Y%m%d_%H%M%S")"
    BE_LOG="${AUTOMATION_LOG_DIR}/multi_session_${session_count}sessions_${POLLER_INTERVAL_MS}ms_${run_stamp}.log"

    log "Khởi động Backend: interval=${POLLER_INTERVAL_MS} ms, session count=${session_count}"
    log "Backend log: ${BE_LOG}"

    (
        cd "${BE_DIR}"

        export SPRING_PROFILES_ACTIVE="measurement"
        export UCON_TRIGGER_POLLER_INTERVAL_MS="${POLLER_INTERVAL_MS}"

        exec "${MAVEN_CMD}" spring-boot:run
    ) >"${BE_LOG}" 2>&1 &

    BE_PID=$!
}


wait_for_backend() {
    local waited=0

    log "Chờ Backend sẵn sàng tại ${BASE_URL}..."

    while [[ "${waited}" -lt "${BACKEND_START_TIMEOUT_SECONDS}" ]]; do
        if ! kill -0 "${BE_PID}" 2>/dev/null; then
            show_backend_log_tail
            die "Backend kết thúc trước khi sẵn sàng."
        fi

        if curl -fsS \
            --max-time 3 \
            "${BASE_URL}/api/measurement/poller/snapshot" \
            >/dev/null 2>&1; then
            log "Backend đã sẵn sàng."
            return
        fi

        sleep 1
        waited=$((waited + 1))
    done

    show_backend_log_tail
    die "Backend không sẵn sàng sau ${BACKEND_START_TIMEOUT_SECONDS} giây."
}


run_measurement_batch() {
    local session_count="$1"
    local batch_name="$2"
    local warmup="$3"
    local iterations="$4"

    log "${batch_name}: sessions=${session_count}, warm-up=${warmup}, iterations=${iterations}"

    (
        cd "${SCRIPT_DIR}"

        "${PYTHON_BIN}" "${RUNNER}" multi-session \
            --interval-ms "${POLLER_INTERVAL_MS}" \
            --session-count "${session_count}" \
            --warmup "${warmup}" \
            --iterations "${iterations}" \
            --pre-event-settle-seconds \
                "${PRE_EVENT_SETTLE_SECONDS}"
    )

    log "${batch_name} hoàn tất."
}


run_session_count() {
    local session_count="$1"

    printf '\n============================================================\n'
    log "BẮT ĐẦU: ${session_count} ACTIVE SESSION(S)"
    printf '============================================================\n'

    ensure_no_existing_backend
    start_backend "${session_count}"
    wait_for_backend

    log "Chờ Backend/JVM ổn định ${STABILIZATION_SECONDS} giây..."
    sleep "${STABILIZATION_SECONDS}"

    run_measurement_batch \
        "${session_count}" \
        "Batch 1" \
        "${BATCH1_WARMUP}" \
        "${BATCH1_ITERATIONS}"

    log "Nghỉ ${BETWEEN_BATCHES_SECONDS} giây trước Batch 2..."
    sleep "${BETWEEN_BATCHES_SECONDS}"

    run_measurement_batch \
        "${session_count}" \
        "Batch 2" \
        "${BATCH2_WARMUP}" \
        "${BATCH2_ITERATIONS}"

    stop_backend

    log "HOÀN TẤT: ${session_count} ACTIVE SESSION(S)"
}


main() {
    check_prerequisites
    BASE_URL="$(load_base_url)"

    log "UCON DB-event multi-session automation"
    log "Measurement directory: ${SCRIPT_DIR}"
    log "Backend directory: ${BE_DIR}"
    log "Base URL: ${BASE_URL}"
    log "Poller interval: ${POLLER_INTERVAL_MS} ms"
    log "Session counts: ${SESSION_COUNTS[*]}"
    log "Batch 1: ${BATCH1_WARMUP} warm-up + ${BATCH1_ITERATIONS} mẫu"
    log "Batch 2: ${BATCH2_WARMUP} warm-up + ${BATCH2_ITERATIONS} mẫu"
    log "Pre-event settle: ${PRE_EVENT_SETTLE_SECONDS} giây"

    local index=0
    local total="${#SESSION_COUNTS[@]}"
    local session_count

    for session_count in "${SESSION_COUNTS[@]}"; do
        index=$((index + 1))
        log "Cấu hình ${index}/${total}"

        run_session_count "${session_count}"

        if [[ "${index}" -lt "${total}" ]]; then
            log "Nghỉ ${BETWEEN_COUNTS_SECONDS} giây trước mức tiếp theo..."
            sleep "${BETWEEN_COUNTS_SECONDS}"
        fi
    done

    printf '\n============================================================\n'
    log "HOÀN TẤT TOÀN BỘ KỊCH BẢN 2"
    log "CSV results: ${RESULTS_DIR}"
    log "Backend logs: ${AUTOMATION_LOG_DIR}"
    printf '============================================================\n'
}


main "$@"