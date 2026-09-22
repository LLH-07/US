#!/usr/bin/env bash
set -Eeuo pipefail

# ============================================================
# UCON timer-expiry automation (Kịch bản 3)
#
# Trình tự cho mỗi timer scheduler interval:
#   1. Khởi động Backend với profile measurement.
#   2. Giữ trigger-event poller interval cố định ở 1000 ms.
#   3. Cấu hình timer scheduler interval cần đo.
#   4. Chờ Backend/JVM ổn định.
#   5. Batch 1: 30 warm-up + 50 mẫu.
#   6. Nghỉ ngắn.
#   7. Batch 2: 10 warm-up + 50 mẫu.
#   8. Dừng Backend.
#
# Mặc định đo scheduler interval:
#   250, 500, 1000, 2000, 5000 ms
#
# Có thể ghi đè, ví dụ smoke test:
#   SCHEDULER_INTERVALS="500 1000" \
#   BATCH1_WARMUP=1 BATCH1_ITERATIONS=2 \
#   BATCH2_WARMUP=0 BATCH2_ITERATIONS=1 \
#   ./run_timer_expiry.sh
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BE_DIR="${SCRIPT_DIR}/../BE"
RUNNER="${SCRIPT_DIR}/measurement_runner.py"
ENV_FILE="${SCRIPT_DIR}/.env"
PYTHON_BIN="${SCRIPT_DIR}/.venv/bin/python"

RESULTS_DIR="${SCRIPT_DIR}/results"
AUTOMATION_LOG_DIR="${RESULTS_DIR}/automation_logs"

POLLER_INTERVAL_MS="${POLLER_INTERVAL_MS:-1000}"
SCHEDULER_INTERVALS_STRING="${SCHEDULER_INTERVALS:-250 500 1000 2000 5000}"
EXPIRY_LEAD_SECONDS="${EXPIRY_LEAD_SECONDS:-3}"

BATCH1_WARMUP="${BATCH1_WARMUP:-30}"
BATCH1_ITERATIONS="${BATCH1_ITERATIONS:-50}"
BATCH2_WARMUP="${BATCH2_WARMUP:-10}"
BATCH2_ITERATIONS="${BATCH2_ITERATIONS:-50}"

STABILIZATION_SECONDS="${STABILIZATION_SECONDS:-15}"
BETWEEN_BATCHES_SECONDS="${BETWEEN_BATCHES_SECONDS:-5}"
BETWEEN_INTERVALS_SECONDS="${BETWEEN_INTERVALS_SECONDS:-5}"
BACKEND_START_TIMEOUT_SECONDS="${BACKEND_START_TIMEOUT_SECONDS:-120}"

read -r -a SCHEDULER_INTERVALS <<< "${SCHEDULER_INTERVALS_STRING}"

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


check_positive_number() {
    local name="$1"
    local value="$2"

    [[ "${value}" =~ ^([0-9]+([.][0-9]*)?|[.][0-9]+)$ ]] \
        || die "${name} phải là số dương, nhận được: ${value}"

    awk -v value="${value}" 'BEGIN { exit !(value > 0) }' \
        || die "${name} phải lớn hơn 0, nhận được: ${value}"
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
    require_file "${SCRIPT_DIR}/measurement_runner.py"
    require_file "${ENV_FILE}"
    require_file "${PYTHON_BIN}"

    command -v curl >/dev/null 2>&1 \
        || die "Không tìm thấy curl trong PATH."

    command -v awk >/dev/null 2>&1 \
        || die "Không tìm thấy awk trong PATH."

    if [[ -x "${BE_DIR}/mvnw" ]]; then
        MAVEN_CMD="${BE_DIR}/mvnw"
    else
        MAVEN_CMD="$(command -v mvn || true)"
        [[ -n "${MAVEN_CMD}" ]] \
            || die "Không tìm thấy mvn và Backend cũng không có mvnw."
    fi

    check_positive_integer "POLLER_INTERVAL_MS" "${POLLER_INTERVAL_MS}"
    check_positive_number "EXPIRY_LEAD_SECONDS" "${EXPIRY_LEAD_SECONDS}"
    check_non_negative_integer "BATCH1_WARMUP" "${BATCH1_WARMUP}"
    check_positive_integer "BATCH1_ITERATIONS" "${BATCH1_ITERATIONS}"
    check_non_negative_integer "BATCH2_WARMUP" "${BATCH2_WARMUP}"
    check_positive_integer "BATCH2_ITERATIONS" "${BATCH2_ITERATIONS}"
    check_positive_integer "STABILIZATION_SECONDS" "${STABILIZATION_SECONDS}"
    check_non_negative_integer "BETWEEN_BATCHES_SECONDS" "${BETWEEN_BATCHES_SECONDS}"
    check_non_negative_integer "BETWEEN_INTERVALS_SECONDS" "${BETWEEN_INTERVALS_SECONDS}"
    check_positive_integer "BACKEND_START_TIMEOUT_SECONDS" "${BACKEND_START_TIMEOUT_SECONDS}"

    if [[ "${#SCHEDULER_INTERVALS[@]}" -eq 0 ]]; then
        die "SCHEDULER_INTERVALS không được rỗng."
    fi

    local interval_ms
    for interval_ms in "${SCHEDULER_INTERVALS[@]}"; do
        check_positive_integer "scheduler interval" "${interval_ms}"
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
        log "Quy trình đo timer-expiry dừng do lỗi."
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
    local scheduler_interval_ms="$1"
    local run_stamp

    run_stamp="$(date +"%Y%m%d_%H%M%S")"
    BE_LOG="${AUTOMATION_LOG_DIR}/timer_expiry_poller${POLLER_INTERVAL_MS}ms_scheduler${scheduler_interval_ms}ms_${run_stamp}.log"

    log "Khởi động Backend: poller=${POLLER_INTERVAL_MS} ms, timer scheduler=${scheduler_interval_ms} ms"
    log "Backend log: ${BE_LOG}"

    (
        cd "${BE_DIR}"

        export SPRING_PROFILES_ACTIVE="measurement"
        export UCON_TRIGGER_POLLER_INTERVAL_MS="${POLLER_INTERVAL_MS}"
        export UCON_TIMER_SCHEDULER_INTERVAL_MS="${scheduler_interval_ms}"

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
    local scheduler_interval_ms="$1"
    local batch_name="$2"
    local warmup="$3"
    local iterations="$4"

    log "${batch_name}: scheduler=${scheduler_interval_ms} ms, warm-up=${warmup}, iterations=${iterations}"

    (
        cd "${SCRIPT_DIR}"

        "${PYTHON_BIN}" "${RUNNER}" timer-expiry \
            --poller-interval-ms "${POLLER_INTERVAL_MS}" \
            --scheduler-interval-ms "${scheduler_interval_ms}" \
            --expiry-lead-seconds "${EXPIRY_LEAD_SECONDS}" \
            --warmup "${warmup}" \
            --iterations "${iterations}"
    )

    log "${batch_name} hoàn tất."
}


run_scheduler_interval() {
    local scheduler_interval_ms="$1"

    printf '\n============================================================\n'
    log "BẮT ĐẦU TIMER SCHEDULER INTERVAL ${scheduler_interval_ms} ms"
    printf '============================================================\n'

    ensure_no_existing_backend
    start_backend "${scheduler_interval_ms}"
    wait_for_backend

    log "Chờ Backend/JVM ổn định ${STABILIZATION_SECONDS} giây..."
    sleep "${STABILIZATION_SECONDS}"

    run_measurement_batch \
        "${scheduler_interval_ms}" \
        "Batch 1" \
        "${BATCH1_WARMUP}" \
        "${BATCH1_ITERATIONS}"

    log "Nghỉ ${BETWEEN_BATCHES_SECONDS} giây trước Batch 2..."
    sleep "${BETWEEN_BATCHES_SECONDS}"

    run_measurement_batch \
        "${scheduler_interval_ms}" \
        "Batch 2" \
        "${BATCH2_WARMUP}" \
        "${BATCH2_ITERATIONS}"

    stop_backend

    log "HOÀN TẤT TIMER SCHEDULER INTERVAL ${scheduler_interval_ms} ms"
}


main() {
    check_prerequisites
    BASE_URL="$(load_base_url)"

    log "UCON timer-expiry automation (Kịch bản 3)"
    log "Measurement directory: ${SCRIPT_DIR}"
    log "Backend directory: ${BE_DIR}"
    log "Base URL: ${BASE_URL}"
    log "Trigger-event poller interval cố định: ${POLLER_INTERVAL_MS} ms"
    log "Timer scheduler intervals: ${SCHEDULER_INTERVALS[*]} ms"
    log "Expiry lead: ${EXPIRY_LEAD_SECONDS} giây + random phase"
    log "Batch 1: ${BATCH1_WARMUP} warm-up + ${BATCH1_ITERATIONS} mẫu"
    log "Batch 2: ${BATCH2_WARMUP} warm-up + ${BATCH2_ITERATIONS} mẫu"

    local index=0
    local total="${#SCHEDULER_INTERVALS[@]}"
    local scheduler_interval_ms

    for scheduler_interval_ms in "${SCHEDULER_INTERVALS[@]}"; do
        index=$((index + 1))
        log "Cấu hình ${index}/${total}"

        run_scheduler_interval "${scheduler_interval_ms}"

        if [[ "${index}" -lt "${total}" ]]; then
            log "Nghỉ ${BETWEEN_INTERVALS_SECONDS} giây trước interval kế tiếp..."
            sleep "${BETWEEN_INTERVALS_SECONDS}"
        fi
    done

    printf '\n============================================================\n'
    log "HOÀN TẤT TOÀN BỘ KỊCH BẢN 3"
    log "CSV results: ${RESULTS_DIR}"
    log "Backend logs: ${AUTOMATION_LOG_DIR}"
    printf '============================================================\n'
}


main "$@"