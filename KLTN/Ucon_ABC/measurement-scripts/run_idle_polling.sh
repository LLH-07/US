#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BE_DIR="${SCRIPT_DIR}/../BE"
RUNNER="${SCRIPT_DIR}/measurement_runner.py"
ENV_FILE="${SCRIPT_DIR}/.env"
PYTHON_BIN="${SCRIPT_DIR}/.venv/bin/python"

RESULTS_DIR="${SCRIPT_DIR}/results"
AUTOMATION_LOG_DIR="${RESULTS_DIR}/automation_logs"

INTERVALS_STRING="${INTERVALS:-250 500 1000 2000 5000}"
DURATION_SECONDS="${DURATION_SECONDS:-300}"
RUNS_PER_INTERVAL="${RUNS_PER_INTERVAL:-1}"
STABILIZATION_SECONDS="${STABILIZATION_SECONDS:-15}"
BETWEEN_RUNS_SECONDS="${BETWEEN_RUNS_SECONDS:-5}"
BETWEEN_INTERVALS_SECONDS="${BETWEEN_INTERVALS_SECONDS:-5}"
BACKEND_START_TIMEOUT_SECONDS="${BACKEND_START_TIMEOUT_SECONDS:-120}"

read -r -a INTERVALS <<< "${INTERVALS_STRING}"

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

load_base_url() {
    local value

    value="$(
        grep -E '^[[:space:]]*UCON_BASE_URL=' "${ENV_FILE}"             | tail -n 1             | cut -d '=' -f 2-             | tr -d '\r'             | xargs
    )"

    if [[ -z "${value}" ]]; then
        value="http://localhost:8081"
    fi

    printf '%s' "${value%/}"
}

check_positive_integer() {
    local name="$1"
    local value="$2"

    [[ "${value}" =~ ^[1-9][0-9]*$ ]]         || die "${name} phải là số nguyên dương, nhận được: ${value}"
}

check_prerequisites() {
    require_directory "${BE_DIR}"
    require_file "${BE_DIR}/pom.xml"
    require_file "${RUNNER}"
    require_file "${ENV_FILE}"
    require_file "${PYTHON_BIN}"

    command -v curl >/dev/null 2>&1         || die "Không tìm thấy curl trong PATH."

    if [[ -x "${BE_DIR}/mvnw" ]]; then
        MAVEN_CMD="${BE_DIR}/mvnw"
    else
        MAVEN_CMD="$(command -v mvn || true)"
        [[ -n "${MAVEN_CMD}" ]]             || die "Không tìm thấy mvn và Backend cũng không có mvnw."
    fi

    check_positive_integer "DURATION_SECONDS" "${DURATION_SECONDS}"
    check_positive_integer "RUNS_PER_INTERVAL" "${RUNS_PER_INTERVAL}"
    check_positive_integer "STABILIZATION_SECONDS" "${STABILIZATION_SECONDS}"

    mkdir -p "${AUTOMATION_LOG_DIR}"
}

show_backend_log_tail() {
    if [[ -n "${BE_LOG}" && -f "${BE_LOG}" ]]; then
        printf '\n----- 80 dòng cuối Backend log -----\n' >&2
        tail -n 80 "${BE_LOG}" >&2 || true
        printf '%s\n\n' '-----------------------------------' >&2
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
        log "Quy trình idle polling dừng do lỗi."
    fi

    exit "${exit_code}"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

ensure_no_existing_backend() {
    if curl -fsS         --max-time 2         "${BASE_URL}/api/measurement/poller/snapshot"         >/dev/null 2>&1; then
        die "Đã có Backend measurement chạy tại ${BASE_URL}. Hãy dừng nó trước."
    fi
}

start_backend() {
    local interval_ms="$1"
    local run_stamp

    run_stamp="$(date +"%Y%m%d_%H%M%S")"
    BE_LOG="${AUTOMATION_LOG_DIR}/idle_backend_${interval_ms}ms_${run_stamp}.log"

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

        if curl -fsS             --max-time 3             "${BASE_URL}/api/measurement/poller/snapshot"             >/dev/null 2>&1; then
            log "Backend đã sẵn sàng."
            return
        fi

        sleep 1
        waited=$((waited + 1))
    done

    show_backend_log_tail
    die "Backend không sẵn sàng sau ${BACKEND_START_TIMEOUT_SECONDS} giây."
}

run_idle_once() {
    local interval_ms="$1"
    local run_no="$2"

    log "Idle run ${run_no}/${RUNS_PER_INTERVAL}: interval=${interval_ms} ms, duration=${DURATION_SECONDS} giây"

    (
        cd "${SCRIPT_DIR}"
        "${PYTHON_BIN}" "${RUNNER}" idle             --interval-ms "${interval_ms}"             --duration-seconds "${DURATION_SECONDS}"
    )

    log "Idle run ${run_no}/${RUNS_PER_INTERVAL} hoàn tất."
}

run_interval() {
    local interval_ms="$1"

    printf '\n============================================================\n'
    log "BẮT ĐẦU IDLE POLLING: ${interval_ms} ms"
    printf '============================================================\n'

    ensure_no_existing_backend
    start_backend "${interval_ms}"
    wait_for_backend

    log "Chờ Backend ổn định ${STABILIZATION_SECONDS} giây..."
    sleep "${STABILIZATION_SECONDS}"

    local run_no
    for ((run_no = 1; run_no <= RUNS_PER_INTERVAL; run_no++)); do
        run_idle_once "${interval_ms}" "${run_no}"

        if [[ "${run_no}" -lt "${RUNS_PER_INTERVAL}" ]]; then
            log "Nghỉ ${BETWEEN_RUNS_SECONDS} giây trước lần idle tiếp theo..."
            sleep "${BETWEEN_RUNS_SECONDS}"
        fi
    done

    stop_backend
    log "HOÀN TẤT IDLE POLLING: ${interval_ms} ms"
}

main() {
    check_prerequisites
    BASE_URL="$(load_base_url)"

    log "UCON idle-polling automation"
    log "Measurement directory: ${SCRIPT_DIR}"
    log "Backend directory: ${BE_DIR}"
    log "Base URL: ${BASE_URL}"
    log "Intervals: ${INTERVALS[*]} ms"
    log "Thời lượng mỗi lần: ${DURATION_SECONDS} giây"
    log "Số lần mỗi interval: ${RUNS_PER_INTERVAL}"

    local index=0
    local total="${#INTERVALS[@]}"
    local interval_ms

    for interval_ms in "${INTERVALS[@]}"; do
        index=$((index + 1))
        log "Cấu hình ${index}/${total}"

        run_interval "${interval_ms}"

        if [[ "${index}" -lt "${total}" ]]; then
            log "Nghỉ ${BETWEEN_INTERVALS_SECONDS} giây trước interval tiếp theo..."
            sleep "${BETWEEN_INTERVALS_SECONDS}"
        fi
    done

    printf '\n============================================================\n'
    log "HOÀN TẤT TOÀN BỘ IDLE POLLING"
    log "JSON results: ${RESULTS_DIR}"
    log "Backend logs: ${AUTOMATION_LOG_DIR}"
    printf '============================================================\n'
}

main "$@"
