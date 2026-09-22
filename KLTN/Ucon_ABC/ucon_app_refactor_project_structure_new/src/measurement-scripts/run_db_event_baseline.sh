#!/usr/bin/env bash
set -Eeuo pipefail

# ============================================================
# UCON DB-event baseline automation
#
# Trình tự cho mỗi poller interval:
#   1. Khởi động Backend với profile measurement.
#   2. Chờ Backend sẵn sàng.
#   3. Chờ ổn định JVM/Backend.
#   4. Batch 1: 10 warm-up + 50 mẫu.
#   5. Nghỉ ngắn.
#   6. Batch 2: 5 warm-up + 50 mẫu.
#   7. Dừng Backend.
#
# Mặc định đo các interval:
#   250, 500, 1000, 2000, 5000 ms
#
# Có thể ghi đè khi chạy, ví dụ:
#   INTERVALS="500 1000" ./run_db_event_baseline.sh
#   BATCH1_ITERATIONS=10 BATCH2_ITERATIONS=10 ./run_db_event_baseline.sh
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BE_DIR="${SCRIPT_DIR}/../BE"
RUNNER="${SCRIPT_DIR}/measurement_runner.py"
ENV_FILE="${SCRIPT_DIR}/.env"
PYTHON_BIN="${SCRIPT_DIR}/.venv/bin/python"

RESULTS_DIR="${SCRIPT_DIR}/results"
AUTOMATION_LOG_DIR="${RESULTS_DIR}/automation_logs"

# Các giá trị có thể ghi đè bằng biến môi trường.
INTERVALS_STRING="${INTERVALS:-250 500 1000 2000 5000}"
BATCH1_WARMUP="${BATCH1_WARMUP:-30}"
BATCH1_ITERATIONS="${BATCH1_ITERATIONS:-50}"
BATCH2_WARMUP="${BATCH2_WARMUP:-10}"
BATCH2_ITERATIONS="${BATCH2_ITERATIONS:-50}"
STABILIZATION_SECONDS="${STABILIZATION_SECONDS:-15}"
BETWEEN_BATCHES_SECONDS="${BETWEEN_BATCHES_SECONDS:-5}"
BETWEEN_INTERVALS_SECONDS="${BETWEEN_INTERVALS_SECONDS:-5}"
BACKEND_START_TIMEOUT_SECONDS="${BACKEND_START_TIMEOUT_SECONDS:-120}"

# Chuyển chuỗi interval thành mảng Bash.
read -r -a INTERVALS <<< "${INTERVALS_STRING}"

BE_PID=""
BE_LOG=""
CURRENT_INTERVAL=""

# Được gán trong check_prerequisites().
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
        || die "Chưa cài curl hoặc curl không có trong PATH."

    if [[ -x "${BE_DIR}/mvnw" ]]; then
        MAVEN_CMD="${BE_DIR}/mvnw"
    else
        MAVEN_CMD="$(command -v mvn || true)"
        [[ -n "${MAVEN_CMD}" ]] \
            || die "Không tìm thấy mvn và BE cũng không có mvnw."
    fi

    mkdir -p "${AUTOMATION_LOG_DIR}"
}

show_backend_log_tail() {
    if [[ -n "${BE_LOG}" && -f "${BE_LOG}" ]]; then
        printf '\n----- 80 dòng cuối của Backend log -----\n' >&2
        tail -n 80 "${BE_LOG}" >&2 || true
        printf '%s\n\n' '----------------------------------------' >&2
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
        while kill -0 "${BE_PID}" 2>/dev/null && [[ "${waited}" -lt 30 ]]; do
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
        log "Quy trình đo dừng do lỗi."
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
    local interval_ms="$1"
    local run_stamp

    run_stamp="$(date +"%Y%m%d_%H%M%S")"
    CURRENT_INTERVAL="${interval_ms}"
    BE_LOG="${AUTOMATION_LOG_DIR}/backend_${interval_ms}ms_${run_stamp}.log"

    log "Khởi động Backend với poller interval = ${interval_ms} ms"
    log "Backend log: ${BE_LOG}"

    (
        cd "${BE_DIR}"

        export SPRING_PROFILES_ACTIVE="measurement"
        export UCON_TRIGGER_POLLER_INTERVAL_MS="${interval_ms}"

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
    local interval_ms="$1"
    local batch_name="$2"
    local warmup="$3"
    local iterations="$4"

    log "${batch_name}: warm-up=${warmup}, iterations=${iterations}"

    (
        cd "${SCRIPT_DIR}"

        "${PYTHON_BIN}" "${RUNNER}" baseline \
            --interval-ms "${interval_ms}" \
            --warmup "${warmup}" \
            --iterations "${iterations}"
    )

    log "${batch_name} hoàn tất."
}

run_interval() {
    local interval_ms="$1"

    printf '\n============================================================\n'
    log "BẮT ĐẦU INTERVAL ${interval_ms} ms"
    printf '============================================================\n'

    ensure_no_existing_backend
    start_backend "${interval_ms}"
    wait_for_backend

    log "Chờ Backend/JVM ổn định ${STABILIZATION_SECONDS} giây..."
    sleep "${STABILIZATION_SECONDS}"

    run_measurement_batch \
        "${interval_ms}" \
        "Batch 1" \
        "${BATCH1_WARMUP}" \
        "${BATCH1_ITERATIONS}"

    log "Nghỉ ${BETWEEN_BATCHES_SECONDS} giây trước Batch 2..."
    sleep "${BETWEEN_BATCHES_SECONDS}"

    run_measurement_batch \
        "${interval_ms}" \
        "Batch 2" \
        "${BATCH2_WARMUP}" \
        "${BATCH2_ITERATIONS}"

    stop_backend

    log "HOÀN TẤT INTERVAL ${interval_ms} ms"
}

main() {
    check_prerequisites
    BASE_URL="$(load_base_url)"

    log "UCON DB-event baseline automation"
    log "Measurement directory: ${SCRIPT_DIR}"
    log "Backend directory: ${BE_DIR}"
    log "Base URL: ${BASE_URL}"
    log "Intervals: ${INTERVALS[*]} ms"
    log "Batch 1: ${BATCH1_WARMUP} warm-up + ${BATCH1_ITERATIONS} mẫu"
    log "Batch 2: ${BATCH2_WARMUP} warm-up + ${BATCH2_ITERATIONS} mẫu"

    local index=0
    local total="${#INTERVALS[@]}"

    for interval_ms in "${INTERVALS[@]}"; do
        index=$((index + 1))

        log "Cấu hình ${index}/${total}"

        run_interval "${interval_ms}"

        if [[ "${index}" -lt "${total}" ]]; then
            log "Nghỉ ${BETWEEN_INTERVALS_SECONDS} giây trước interval kế tiếp..."
            sleep "${BETWEEN_INTERVALS_SECONDS}"
        fi
    done

    printf '\n============================================================\n'
    log "HOÀN TẤT TOÀN BỘ KỊCH BẢN"
    log "CSV: ${RESULTS_DIR}"
    log "Backend logs: ${AUTOMATION_LOG_DIR}"
    printf '============================================================\n'
}

main "$@"
