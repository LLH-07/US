-- PostgreSQL schema draft for MH Library Demo UI mockup
-- Adjusted schema: short_description belongs to books, not book_volumes.

CREATE TABLE users (
  id BIGSERIAL PRIMARY KEY,
  name VARCHAR(255) NOT NULL,
  email VARCHAR(255) NOT NULL UNIQUE,
  hashed_password TEXT NOT NULL,
  status VARCHAR(50) NOT NULL
);

CREATE TABLE books (
  id BIGSERIAL PRIMARY KEY,
  title VARCHAR(255) NOT NULL,
  genre VARCHAR(120),
  authors VARCHAR(255),
  short_description TEXT,
  default_access_model VARCHAR(50) NOT NULL,
  status VARCHAR(50) NOT NULL
);

CREATE TABLE book_volumes (
  id BIGSERIAL PRIMARY KEY,
  book_id BIGINT NOT NULL REFERENCES books(id),
  title VARCHAR(255) NOT NULL,
  volume_no INTEGER NOT NULL,
  price NUMERIC(12,2) DEFAULT 0,
  access_model_override VARCHAR(50),
  status VARCHAR(50) NOT NULL
);

CREATE TABLE volume_purchases (
  id BIGSERIAL PRIMARY KEY,
  user_id BIGINT NOT NULL REFERENCES users(id),
  volume_id BIGINT NOT NULL REFERENCES book_volumes(id),
  price NUMERIC(12,2) NOT NULL,
  purchased_at TIMESTAMP NOT NULL
);

CREATE TABLE rental_plans (
  id BIGSERIAL PRIMARY KEY,
  name VARCHAR(255) NOT NULL,
  duration_days INTEGER NOT NULL,
  price NUMERIC(12,2) NOT NULL
);

CREATE TABLE user_rentals (
  id BIGSERIAL PRIMARY KEY,
  user_id BIGINT NOT NULL REFERENCES users(id),
  plan_id BIGINT NOT NULL REFERENCES rental_plans(id),
  started_at TIMESTAMP NOT NULL,
  expires_at TIMESTAMP NOT NULL,
  status VARCHAR(50) NOT NULL
);

CREATE TABLE family_groups (
  id BIGSERIAL PRIMARY KEY,
  owner_user_id BIGINT NOT NULL REFERENCES users(id),
  name VARCHAR(255) NOT NULL,
  status VARCHAR(50) NOT NULL
);

CREATE TABLE family_group_members (
  id BIGSERIAL PRIMARY KEY,
  group_id BIGINT NOT NULL REFERENCES family_groups(id),
  user_id BIGINT NOT NULL REFERENCES users(id),
  status VARCHAR(50) NOT NULL,
  joined_at TIMESTAMP NOT NULL,
  removed_at TIMESTAMP
);

CREATE TABLE family_shared_volumes (
  id BIGSERIAL PRIMARY KEY,
  group_id BIGINT NOT NULL REFERENCES family_groups(id),
  volume_id BIGINT NOT NULL REFERENCES book_volumes(id),
  status VARCHAR(50) NOT NULL,
  shared_at TIMESTAMP NOT NULL,
  revoked_at TIMESTAMP
);

CREATE TABLE policy_registry (
  id BIGSERIAL PRIMARY KEY,
  policy_ref VARCHAR(255) NOT NULL,
  policy_name VARCHAR(255) NOT NULL,
  resource_type VARCHAR(100) NOT NULL,
  action_id VARCHAR(100) NOT NULL,
  access_path VARCHAR(100),
  phase VARCHAR(50) NOT NULL,
  enabled BOOLEAN NOT NULL DEFAULT TRUE,
  description TEXT
);

CREATE TABLE policy_attribute_requirements (
  id BIGSERIAL PRIMARY KEY,
  policy_id BIGINT NOT NULL REFERENCES policy_registry(id),
  attr_key VARCHAR(255) NOT NULL,
  usage_type VARCHAR(100) NOT NULL,
  required BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE TABLE pip_attribute_mappings (
  attr_key VARCHAR(255) PRIMARY KEY,
  xacml_attribute_id VARCHAR(255) NOT NULL,
  xacml_category VARCHAR(255) NOT NULL,
  xacml_data_type VARCHAR(255) NOT NULL,
  reader_name VARCHAR(255),
  writer_name VARCHAR(255),
  dependency_name VARCHAR(255),
  monitor_type VARCHAR(100),
  mutable BOOLEAN NOT NULL DEFAULT FALSE,
  enabled BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE TABLE usage_sessions (
  id BIGSERIAL PRIMARY KEY,
  subject_user_id BIGINT NOT NULL REFERENCES users(id),
  resource_type VARCHAR(100) NOT NULL,
  resource_id BIGINT NOT NULL,
  action_id VARCHAR(100) NOT NULL,
  access_path VARCHAR(100),
  phase VARCHAR(50) NOT NULL,
  status VARCHAR(50) NOT NULL,
  created_at TIMESTAMP NOT NULL,
  activated_at TIMESTAMP,
  last_eval_at TIMESTAMP,
  ended_at TIMESTAMP,
  revoked_at TIMESTAMP,
  revoked_reason TEXT,
  post_handled_at TIMESTAMP
);

CREATE TABLE session_events (
  id BIGSERIAL PRIMARY KEY,
  usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),
  event_type VARCHAR(100) NOT NULL,
  phase VARCHAR(50),
  decision VARCHAR(50),
  reason_code VARCHAR(100),
  event_at TIMESTAMP NOT NULL,
  details_json JSONB
);

CREATE TABLE usage_session_dependencies (
  id BIGSERIAL PRIMARY KEY,
  usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),
  dependency_key VARCHAR(255) NOT NULL,
  entity_type VARCHAR(100) NOT NULL,
  entity_id BIGINT NOT NULL,
  created_at TIMESTAMP NOT NULL,
  active_flag BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE TABLE usage_session_policies (
  id BIGSERIAL PRIMARY KEY,
  usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),
  policy_id BIGINT NOT NULL REFERENCES policy_registry(id),
  bound_at TIMESTAMP NOT NULL
);

CREATE TABLE obligation_executions (
  id BIGSERIAL PRIMARY KEY,
  usage_session_id BIGINT NOT NULL REFERENCES usage_sessions(id),
  policy_id BIGINT REFERENCES policy_registry(id),
  obligation_type VARCHAR(100) NOT NULL,
  target_type VARCHAR(100),
  status VARCHAR(50) NOT NULL,
  payload_json JSONB,
  created_at TIMESTAMP NOT NULL,
  completed_at TIMESTAMP,
  error_message TEXT
);

CREATE TABLE ucon_trigger_events (
  id BIGSERIAL PRIMARY KEY,
  source_type VARCHAR(100) NOT NULL,
  entity_type VARCHAR(100) NOT NULL,
  entity_id BIGINT NOT NULL,
  attribute_name VARCHAR(255) NOT NULL,
  event_type VARCHAR(100) NOT NULL,
  occurred_at TIMESTAMP NOT NULL,
  payload_json JSONB,
  processed_flag BOOLEAN NOT NULL DEFAULT FALSE,
  processed_at TIMESTAMP
);

CREATE TABLE system_parameters (
  id BIGSERIAL PRIMARY KEY,
  parameter_name VARCHAR(255) NOT NULL,
  value TEXT,
  unit VARCHAR(100)
);
