// Schema Admin mô phỏng theo BE/src/main/resources/schema.sql.
// Dùng để render danh sách bảng/cột giống phpMyAdmin/DBeaver mini.
window.ADMIN_SCHEMAS = {
  "USERS": [
    "id",
    "name",
    "email",
    "hashed_password",
    "status"
  ],
  "BOOKS": [
    "id",
    "title",
    "genre",
    "authors",
    "default_access_model",
    "status"
  ],
  "BOOK_VOLUMES": [
    "id",
    "book_id",
    "title",
    "volume_no",
    "price",
    "read_count",
    "access_model_override",
    "status"
  ],
  "RENTAL_PLANS": [
    "id",
    "name",
    "duration_days",
    "price"
  ],
  "USER_RENTALS": [
    "id",
    "user_id",
    "plan_id",
    "started_at",
    "expires_at",
    "status"
  ],
  "VOLUME_PURCHASES": [
    "id",
    "user_id",
    "volume_id",
    "price",
    "purchased_at"
  ],
  "FAMILY_GROUPS": [
    "id",
    "owner_user_id",
    "name",
    "status"
  ],
  "FAMILY_GROUP_MEMBERS": [
    "id",
    "group_id",
    "user_id",
    "status",
    "joined_at",
    "removed_at"
  ],
  "FAMILY_SHARED_VOLUMES": [
    "id",
    "group_id",
    "volume_id",
    "status",
    "shared_at",
    "revoked_at"
  ],
  "SYSTEM_PARAMETERS": [
    "id",
    "parameter_name",
    "value",
    "unit"
  ],
  "POLICY_REGISTRY": [
    "id",
    "policy_ref",
    "policy_name",
    "resource_type",
    "action_id",
    "access_path",
    "phase",
    "enabled",
    "description"
  ],
  "PIP_ATTRIBUTE_MAPPINGS": [
    "attr_key",
    "xacml_attribute_id",
    "xacml_category",
    "xacml_data_type",
    "reader_name",
    "writer_name",
    "dependency_name",
    "monitor_type",
    "mutable",
    "enabled"
  ],
  "PIP_UPDATE_ACTIONS": [
    "action_code",
    "attr_key",
    "handler_name",
    "operation",
    "default_value",
    "enabled"
  ],
  "POLICY_ATTRIBUTE_REQUIREMENTS": [
    "id",
    "policy_id",
    "attr_key",
    "usage_type",
    "required"
  ],
  "USAGE_SESSIONS": [
    "id",
    "subject_user_id",
    "resource_type",
    "resource_id",
    "action_id",
    "access_path",
    "phase",
    "status",
    "created_at",
    "activated_at",
    "last_eval_at",
    "ended_at",
    "revoked_at",
    "revoked_reason",
    "post_handled_at"
  ],
  "USAGE_SESSION_POLICIES": [
    "id",
    "usage_session_id",
    "policy_id",
    "bound_at"
  ],
  "USAGE_SESSION_DEPENDENCIES": [
    "id",
    "usage_session_id",
    "dependency_key",
    "entity_type",
    "entity_id",
    "created_at",
    "active_flag"
  ],
  "UCON_TRIGGER_EVENTS": [
    "id",
    "source_type",
    "entity_type",
    "entity_id",
    "attribute_name",
    "event_type",
    "occurred_at",
    "payload_json",
    "processed_flag",
    "processed_at"
  ],
  "SESSION_EVENTS": [
    "id",
    "usage_session_id",
    "event_type",
    "phase",
    "decision",
    "reason_code",
    "event_at",
    "details_json"
  ],
  "OBLIGATION_EXECUTIONS": [
    "id",
    "usage_session_id",
    "policy_id",
    "obligation_type",
    "target_type",
    "status",
    "payload_json",
    "created_at",
    "completed_at",
    "error_message"
  ]
};
