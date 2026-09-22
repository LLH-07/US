DROP TABLE IF EXISTS session_events CASCADE@@
DROP TABLE IF EXISTS ucon_trigger_events CASCADE@@
DROP TABLE IF EXISTS usage_session_dependencies CASCADE@@
DROP TABLE IF EXISTS usage_session_policies CASCADE@@
DROP TABLE IF EXISTS usage_sessions CASCADE@@

DROP TABLE IF EXISTS policy_attribute_requirements CASCADE@@
DROP TABLE IF EXISTS pip_update_actions CASCADE@@
DROP TABLE IF EXISTS pip_attribute_mappings CASCADE@@
DROP TABLE IF EXISTS obligation_evidence CASCADE@@
DROP TABLE IF EXISTS obligation_catalog CASCADE@@
DROP TABLE IF EXISTS policy_registry CASCADE@@

DROP TABLE IF EXISTS family_shared_volumes CASCADE@@
DROP TABLE IF EXISTS family_group_members CASCADE@@
DROP TABLE IF EXISTS family_groups CASCADE@@

DROP TABLE IF EXISTS volume_purchases CASCADE@@
DROP TABLE IF EXISTS user_rentals CASCADE@@
DROP TABLE IF EXISTS rental_plans CASCADE@@
DROP TABLE IF EXISTS book_volumes CASCADE@@
DROP TABLE IF EXISTS books CASCADE@@
DROP TABLE IF EXISTS users CASCADE@@
DROP TABLE IF EXISTS system_parameters CASCADE@@

CREATE TABLE users (
    id              BIGSERIAL PRIMARY KEY,
    name            VARCHAR(100) NOT NULL,
    email           VARCHAR(150) NOT NULL UNIQUE,
    hashed_password VARCHAR(255),
    status          VARCHAR(30) NOT NULL
)@@

CREATE TABLE books (
    id                   BIGSERIAL PRIMARY KEY,
    title                VARCHAR(255) NOT NULL,
    genre                VARCHAR(100),
    authors              VARCHAR(255),
    default_access_model VARCHAR(30) NOT NULL,
    status               VARCHAR(30) NOT NULL
)@@

CREATE TABLE book_volumes (
    id                    BIGSERIAL PRIMARY KEY,
    book_id               BIGINT NOT NULL REFERENCES books(id),
    title                 VARCHAR(255) NOT NULL,
    volume_no             INT NOT NULL,
    price                 NUMERIC(12,2),
    content_path          VARCHAR(500) NOT NULL,
    read_count            BIGINT NOT NULL DEFAULT 0,
    access_model_override VARCHAR(30) NOT NULL,
    status                VARCHAR(30) NOT NULL
)@@

CREATE TABLE rental_plans (
    id            BIGSERIAL PRIMARY KEY,
    name          VARCHAR(100) NOT NULL,
    duration_days INT NOT NULL,
    price         NUMERIC(12,2) NOT NULL
)@@

CREATE TABLE user_rentals (
    id         BIGSERIAL PRIMARY KEY,
    user_id    BIGINT NOT NULL REFERENCES users(id),
    plan_id    BIGINT NOT NULL REFERENCES rental_plans(id),
    started_at TIMESTAMP NOT NULL,
    expires_at TIMESTAMP NOT NULL,
    status     VARCHAR(30) NOT NULL
)@@

CREATE TABLE volume_purchases (
    id           BIGSERIAL PRIMARY KEY,
    user_id      BIGINT NOT NULL REFERENCES users(id),
    volume_id    BIGINT NOT NULL REFERENCES book_volumes(id),
    price        NUMERIC(12,2) NOT NULL,
    purchased_at TIMESTAMP NOT NULL DEFAULT now()
)@@

CREATE TABLE family_groups (
    id            BIGSERIAL PRIMARY KEY,
    owner_user_id BIGINT NOT NULL REFERENCES users(id),
    name          VARCHAR(100) NOT NULL,
    status        VARCHAR(30) NOT NULL
)@@

CREATE TABLE family_group_members (
    id         BIGSERIAL PRIMARY KEY,
    group_id   BIGINT NOT NULL REFERENCES family_groups(id),
    user_id    BIGINT NOT NULL REFERENCES users(id),
    status     VARCHAR(30) NOT NULL,
    joined_at  TIMESTAMP NOT NULL DEFAULT now(),
    removed_at TIMESTAMP
)@@

CREATE TABLE family_shared_volumes (
    id         BIGSERIAL PRIMARY KEY,
    group_id   BIGINT NOT NULL REFERENCES family_groups(id),
    volume_id  BIGINT NOT NULL REFERENCES book_volumes(id),
    status     VARCHAR(30) NOT NULL,
    shared_at  TIMESTAMP NOT NULL DEFAULT now(),
    revoked_at TIMESTAMP
)@@

CREATE TABLE system_parameters (
    id             BIGSERIAL PRIMARY KEY,
    parameter_name VARCHAR(100) NOT NULL UNIQUE,
    value          VARCHAR(255) NOT NULL,
    unit           VARCHAR(50)
)@@

CREATE TABLE policy_registry (
    id            BIGSERIAL PRIMARY KEY,
    policy_ref    VARCHAR(100) NOT NULL UNIQUE,
    policy_name   VARCHAR(255) NOT NULL,
    resource_type VARCHAR(50) NOT NULL,
    action_id     VARCHAR(50) NOT NULL,
    access_path   VARCHAR(50) NOT NULL,
    phase         VARCHAR(50) NOT NULL,
    enabled       BOOLEAN NOT NULL DEFAULT TRUE,
    candidate_enabled BOOLEAN NOT NULL DEFAULT TRUE,
    priority INT NOT NULL DEFAULT 100,
    description   TEXT
)@@

CREATE TABLE obligation_catalog (
    obligation_id   VARCHAR(100) PRIMARY KEY,
    version         VARCHAR(30) NOT NULL,
    obligation_type VARCHAR(50) NOT NULL,
    scope           VARCHAR(50) NOT NULL,
    title           VARCHAR(255) NOT NULL,
    description     TEXT,
    enabled         BOOLEAN NOT NULL DEFAULT TRUE,

    CHECK (scope IN ('USER_POLICY', 'USER_GLOBAL'))
)@@

CREATE TABLE obligation_evidence (
    id                  BIGSERIAL PRIMARY KEY,

    obligation_id       VARCHAR(100) NOT NULL
                        REFERENCES obligation_catalog(obligation_id),

    obligation_version  VARCHAR(30) NOT NULL,

    subject_user_id     BIGINT NOT NULL
                        REFERENCES users(id),

    policy_ref          VARCHAR(100)
                        REFERENCES policy_registry(policy_ref),

    status              VARCHAR(30) NOT NULL,
    fulfilled_at        TIMESTAMP NOT NULL DEFAULT clock_timestamp(),
    expires_at          TIMESTAMP,
    evidence_json       JSONB,

    CHECK (status IN ('FULFILLED', 'REVOKED'))
)@@

-- Unique cho USER_GLOBAL
-- Mỗi user chỉ có một evidence hiện hành cho một phiên bản obligation toàn cục.
CREATE UNIQUE INDEX uq_obligation_evidence_user_global
ON obligation_evidence(obligation_id, obligation_version, subject_user_id)
WHERE policy_ref IS NULL@@

-- Unique cho USER_POLICY
-- Mỗi user có một evidence cho obligation đó trong từng policy.
CREATE UNIQUE INDEX uq_obligation_evidence_user_policy
ON obligation_evidence(obligation_id, obligation_version, subject_user_id, policy_ref)
WHERE policy_ref IS NOT NULL@@

CREATE TABLE pip_attribute_mappings (
    attr_key            VARCHAR(150) PRIMARY KEY,

    xacml_attribute_id  VARCHAR(255) NOT NULL,
    xacml_category      VARCHAR(255) NOT NULL,
    xacml_data_type     VARCHAR(255) NOT NULL,

    reader_name         VARCHAR(150) NOT NULL,
    dependency_name     VARCHAR(150),
    monitor_type        VARCHAR(50) NOT NULL DEFAULT 'NONE',

    mutable             BOOLEAN NOT NULL DEFAULT FALSE,
    enabled             BOOLEAN NOT NULL DEFAULT TRUE
)@@

CREATE TABLE pip_update_actions (
    action_code   VARCHAR(150) PRIMARY KEY,
    attr_key      VARCHAR(150) NOT NULL REFERENCES pip_attribute_mappings(attr_key),
    handler_name  VARCHAR(150) NOT NULL,
    operation     VARCHAR(50) NOT NULL,
    default_value VARCHAR(100),
    enabled       BOOLEAN NOT NULL DEFAULT TRUE
)@@

CREATE TABLE policy_attribute_requirements (
    id          BIGSERIAL PRIMARY KEY,
    policy_id   BIGINT NOT NULL REFERENCES policy_registry(id),
    attr_key    VARCHAR(150) NOT NULL REFERENCES pip_attribute_mappings(attr_key),
    usage_type  VARCHAR(50) NOT NULL,
    required    BOOLEAN NOT NULL DEFAULT TRUE
)@@

CREATE TABLE usage_sessions (
    id              BIGSERIAL PRIMARY KEY,
    subject_user_id BIGINT NOT NULL REFERENCES users(id),
    resource_type   VARCHAR(50) NOT NULL,
    resource_id     BIGINT NOT NULL,
    action_id       VARCHAR(50) NOT NULL,
    access_path     VARCHAR(50) NOT NULL,
    phase           VARCHAR(50) NOT NULL,
    status          VARCHAR(50) NOT NULL,

    created_at      TIMESTAMP NOT NULL DEFAULT clock_timestamp(), -- thời điểm tạo session PENDING
    activated_at    TIMESTAMP,                        -- thời điểm chuyển sang ACTIVE
    last_eval_at    TIMESTAMP,                        -- lần đánh giá gần nhất

    ended_at        TIMESTAMP,
    revoked_at      TIMESTAMP,
    revoked_reason  VARCHAR(150),
    post_handled_at TIMESTAMP
)@@

CREATE TABLE usage_session_policies (
    id               BIGSERIAL PRIMARY KEY,
    usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),
    policy_id         BIGINT NOT NULL REFERENCES policy_registry(id),
    bound_at          TIMESTAMP NOT NULL DEFAULT clock_timestamp()
)@@

CREATE TABLE usage_session_dependencies (
    id               BIGSERIAL PRIMARY KEY,
    usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),

    dependency_key   VARCHAR(150) NOT NULL,
    attr_key         VARCHAR(150) NOT NULL,

    entity_type      VARCHAR(80) NOT NULL,
    entity_id        BIGINT NOT NULL,
    attribute_name   VARCHAR(100) NOT NULL,

    monitor_type     VARCHAR(50) NOT NULL DEFAULT 'NONE',
    next_check_at    TIMESTAMP,

    created_at       TIMESTAMP NOT NULL DEFAULT clock_timestamp(),
    active_flag      BOOLEAN NOT NULL DEFAULT TRUE,

    UNIQUE (
        usage_session_id,
        attr_key,
        entity_type,
        entity_id,
        attribute_name
    )
)@@

CREATE TABLE ucon_trigger_events (
    id             BIGSERIAL PRIMARY KEY,
    source_type    VARCHAR(50) NOT NULL,
    entity_type    VARCHAR(80) NOT NULL,
    entity_id      BIGINT NOT NULL,
    attribute_name VARCHAR(100),
    event_type     VARCHAR(80) NOT NULL,
    occurred_at    TIMESTAMP NOT NULL DEFAULT clock_timestamp(),
    payload_json   JSONB,
    processed_flag BOOLEAN NOT NULL DEFAULT FALSE,
    picked_at   TIMESTAMP DEFAULT NULL,
    processed_at   TIMESTAMP DEFAULT NULL
)@@

CREATE TABLE session_events (
    id                BIGSERIAL PRIMARY KEY,
    usage_session_id  BIGINT REFERENCES usage_sessions(id),
    trigger_event_id  BIGINT,
    event_type        VARCHAR(80) NOT NULL,
    phase             VARCHAR(50),
    decision          VARCHAR(50),
    reason_code       VARCHAR(150),
    event_at          TIMESTAMP NOT NULL DEFAULT clock_timestamp(),
    logged_at         TIMESTAMP NOT NULL DEFAULT clock_timestamp(),
    details_json      JSONB
)@@

CREATE INDEX idx_user_rentals_user_status_expires
ON user_rentals(user_id, status, expires_at)@@

CREATE INDEX idx_policy_registry_lookup
ON policy_registry(resource_type, action_id, access_path, phase, enabled)@@

CREATE INDEX idx_obligation_evidence_lookup
ON obligation_evidence(obligation_id, subject_user_id, policy_ref, fulfilled_at DESC)@@

CREATE INDEX idx_usage_session_policies_session
ON usage_session_policies(usage_session_id)@@

CREATE INDEX idx_session_dependencies_event_lookup
ON usage_session_dependencies(entity_type, entity_id, attribute_name, active_flag)@@

CREATE INDEX idx_session_dependencies_timer_lookup
ON usage_session_dependencies(next_check_at, active_flag)
WHERE monitor_type IN ('TIMER', 'HYBRID')@@

CREATE INDEX idx_trigger_events_unprocessed
ON ucon_trigger_events(processed_flag, occurred_at)@@

CREATE INDEX idx_session_events_trigger_event
ON session_events(trigger_event_id, event_type)
WHERE trigger_event_id IS NOT NULL@@

CREATE OR REPLACE FUNCTION fn_ucon_generic_dependency_event()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_old_row JSONB;
    v_new_row JSONB;
    v_changed RECORD;
    v_dep RECORD;
    v_entity_type VARCHAR(80);
    v_entity_id BIGINT;
BEGIN
    v_entity_type := upper(TG_TABLE_NAME);

    IF TG_OP = 'UPDATE' THEN
        v_old_row := to_jsonb(OLD);
        v_new_row := to_jsonb(NEW);
        v_entity_id := (v_new_row ->> 'id')::BIGINT;

        FOR v_changed IN
            SELECT n.key AS attribute_name,
                   o.value AS old_value,
                   n.value AS new_value
            FROM jsonb_each(v_new_row) n
            JOIN jsonb_each(v_old_row) o
              ON o.key = n.key
            WHERE o.value IS DISTINCT FROM n.value
        LOOP
            IF NOT EXISTS (
                SELECT 1
                FROM usage_session_dependencies dep
                JOIN usage_sessions us
                  ON us.id = dep.usage_session_id
                WHERE us.status = 'ACTIVE'
                  AND dep.active_flag = true
                  AND dep.monitor_type IN ('DB_EVENT', 'HYBRID')
                  AND dep.entity_type = v_entity_type
                  AND dep.entity_id = v_entity_id
                  AND dep.attribute_name = v_changed.attribute_name
                LIMIT 1
            ) THEN
                CONTINUE;
            END IF;

            INSERT INTO ucon_trigger_events(
                source_type,
                entity_type,
                entity_id,
                attribute_name,
                event_type,
                payload_json
            )
            VALUES (
                'DB_TRIGGER',
                v_entity_type,
                v_entity_id,
                v_changed.attribute_name,
                'ATTRIBUTE_CHANGED',
                jsonb_build_object(
                    'operation', TG_OP,
                    'table_name', TG_TABLE_NAME,
                    'column_name', v_changed.attribute_name,
                    'old_value', v_changed.old_value,
                    'new_value', v_changed.new_value
                )
            );
        END LOOP;

        RETURN NEW;
    END IF;

    IF TG_OP = 'DELETE' THEN
        v_old_row := to_jsonb(OLD);
        v_entity_id := (v_old_row ->> 'id')::BIGINT;

        /*
         * Khi row bị DELETE, không còn khái niệm "cột nào thay đổi".
         * Vì vậy ta tạo event cho từng attribute đang được session active monitor
         * trên chính entity bị xóa.
         *
         * Cách này giúp luồng xử lý cũ vẫn tìm được session bị ảnh hưởng dựa trên:
         * entity_type + entity_id + attribute_name.
         */
        FOR v_dep IN
            SELECT DISTINCT dep.attribute_name
            FROM usage_session_dependencies dep
            JOIN usage_sessions us
              ON us.id = dep.usage_session_id
            WHERE us.status = 'ACTIVE'
              AND dep.active_flag = true
              AND dep.monitor_type IN ('DB_EVENT', 'HYBRID')
              AND dep.entity_type = v_entity_type
              AND dep.entity_id = v_entity_id
        LOOP
            INSERT INTO ucon_trigger_events(
                source_type,
                entity_type,
                entity_id,
                attribute_name,
                event_type,
                payload_json
            )
            VALUES (
                'DB_TRIGGER',
                v_entity_type,
                v_entity_id,
                v_dep.attribute_name,
                'ENTITY_DELETED',
                jsonb_build_object(
                    'operation', TG_OP,
                    'table_name', TG_TABLE_NAME,
                    'column_name', v_dep.attribute_name,
                    'old_value', v_old_row -> v_dep.attribute_name,
                    'new_value', NULL,
                    'old_row', v_old_row
                )
            );
        END LOOP;

        RETURN OLD;
    END IF;

    RETURN NULL;
END;
$$@@

DROP TRIGGER IF EXISTS trg_ucon_user_rentals_event ON user_rentals@@

CREATE TRIGGER trg_ucon_user_rentals_event
AFTER UPDATE OR DELETE ON user_rentals
FOR EACH ROW
EXECUTE FUNCTION fn_ucon_generic_dependency_event()@@


DROP TRIGGER IF EXISTS trg_ucon_book_volumes_event ON book_volumes@@

CREATE TRIGGER trg_ucon_book_volumes_event
AFTER UPDATE OR DELETE ON book_volumes
FOR EACH ROW
EXECUTE FUNCTION fn_ucon_generic_dependency_event()@@


DROP TRIGGER IF EXISTS trg_ucon_family_groups_event ON family_groups@@

CREATE TRIGGER trg_ucon_family_groups_event
AFTER UPDATE OR DELETE ON family_groups
FOR EACH ROW
EXECUTE FUNCTION fn_ucon_generic_dependency_event()@@


DROP TRIGGER IF EXISTS trg_ucon_family_group_members_event ON family_group_members@@

CREATE TRIGGER trg_ucon_family_group_members_event
AFTER UPDATE OR DELETE ON family_group_members
FOR EACH ROW
EXECUTE FUNCTION fn_ucon_generic_dependency_event()@@


DROP TRIGGER IF EXISTS trg_ucon_family_shared_volumes_event ON family_shared_volumes@@

CREATE TRIGGER trg_ucon_family_shared_volumes_event
AFTER UPDATE OR DELETE ON family_shared_volumes
FOR EACH ROW
EXECUTE FUNCTION fn_ucon_generic_dependency_event()@@