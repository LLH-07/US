package vn.ucon.demo.ucon.sm;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Map;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.MonitorType;
import vn.ucon.demo.ucon.model.PolicyMetadata;
import vn.ucon.demo.ucon.model.SessionDependency;

@Component
public class UsageSessionManager {

    private final JdbcTemplate jdbcTemplate;

    public UsageSessionManager(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Transactional
    public Map.Entry<Long, LocalDateTime> createPendingSession(
            AccessRequest request,
            List<PolicyMetadata> boundPolicies,
            List<SessionDependency> dependencies,
            LocalDateTime evaluatedAt
    ) {
        Map.Entry<Long, LocalDateTime> createdSession =
        jdbcTemplate.queryForObject("""
                WITH creation_time AS (
                    SELECT clock_timestamp()::timestamp AS value
                )
                INSERT INTO usage_sessions(
                    subject_user_id,
                    resource_type,
                    resource_id,
                    action_id,
                    access_path,
                    phase,
                    status,
                    created_at,
                    last_eval_at
                )
                SELECT
                    ?, ?, ?, ?, ?,
                    'PRE',
                    'PENDING',
                    creation_time.value,
                    ?
                FROM creation_time
                RETURNING id, created_at
                """,
                (rs, rowNum) -> Map.entry(
                        rs.getLong("id"),
                        rs.getTimestamp("created_at").toLocalDateTime()
                ),
                request.subjectUserId(),
                request.resourceType(),
                request.resourceId(),
                request.actionId(),
                request.accessPath(),
                evaluatedAt
        );

        Long sessionId = createdSession.getKey();

        for (PolicyMetadata policy : boundPolicies) {
            jdbcTemplate.update("""
                INSERT INTO usage_session_policies(
                    usage_session_id,
                    policy_id
                )
                VALUES (?, ?)
            """, sessionId, policy.policyId());
        }

        saveDependencies(sessionId, dependencies);

        return createdSession;
    }

    private void saveDependencies(
            Long sessionId,
            List<SessionDependency> dependencies
    ) {
        for (SessionDependency dependency : dependencies.stream().distinct().toList()) {
            jdbcTemplate.update("""
                INSERT INTO usage_session_dependencies(
                    usage_session_id,
                    dependency_key,
                    attr_key,
                    entity_type,
                    entity_id,
                    attribute_name,
                    monitor_type,
                    next_check_at
                )
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                ON CONFLICT (
                    usage_session_id,
                    attr_key,
                    entity_type,
                    entity_id,
                    attribute_name
                )
                DO UPDATE SET
                    monitor_type = EXCLUDED.monitor_type,
                    next_check_at = EXCLUDED.next_check_at,
                    active_flag = true
            """,
                    sessionId,
                    dependency.dependencyKey(),
                    dependency.attrKey(),
                    dependency.entityType(),
                    dependency.entityId(),
                    dependency.attributeName(),
                    dependency.monitorType(),
                    dependency.nextCheckAt()
            );
        }
    }

   @Transactional
    public LocalDateTime activateSession(
            Long sessionId,
            LocalDateTime evaluatedAt
    ) {
        return jdbcTemplate.queryForObject("""
            WITH transition_time AS (
                SELECT clock_timestamp()::timestamp AS value
            )
            UPDATE usage_sessions us
            SET status = 'ACTIVE',
                phase = 'ONGOING',
                activated_at = transition_time.value,
                last_eval_at = ?
            FROM transition_time
            WHERE us.id = ?
            AND us.status = 'PENDING'
            RETURNING us.activated_at
            """,
            LocalDateTime.class,
            evaluatedAt,
            sessionId
        );
    }

    public boolean isPending(Long sessionId) {
        Integer count = jdbcTemplate.queryForObject("""
            SELECT COUNT(*)
            FROM usage_sessions
            WHERE id = ?
              AND status = 'PENDING'
        """, Integer.class, sessionId);

        return count != null && count > 0;
    }

    public boolean isActive(Long sessionId) {
        Integer count = jdbcTemplate.queryForObject("""
            SELECT COUNT(*)
            FROM usage_sessions
            WHERE id = ?
              AND status = 'ACTIVE'
        """, Integer.class, sessionId);

        return count != null && count > 0;
    }

    @Transactional
    public LocalDateTime denyPendingSession(
            Long sessionId,
            String reasonCode,
            LocalDateTime evaluatedAt
    ) {
        LocalDateTime deniedAt = jdbcTemplate.queryForObject("""
            WITH transition_time AS (
                SELECT clock_timestamp()::timestamp AS value
            )
            UPDATE usage_sessions us
            SET status = 'ACTIVATION_DENIED',
                phase = 'POST',
                last_eval_at = ?,
                ended_at = transition_time.value,
                revoked_reason = ?
            FROM transition_time
            WHERE us.id = ?
            AND us.status = 'PENDING'
            RETURNING us.ended_at
            """,
            LocalDateTime.class,
            evaluatedAt,
            reasonCode,
            sessionId
        );

        // deactivateDependencies(sessionId);

        return deniedAt;
    }

    public Map<String, Object> findSession(Long sessionId) {
        return jdbcTemplate.queryForMap("""
            SELECT *
            FROM usage_sessions
            WHERE id = ?
        """, sessionId);
    }

    public Map<String, Object> findSessionWithBoundPolicies(Long sessionId) {
        return jdbcTemplate.queryForMap("""
            SELECT
                us.*,
                bound_policies.pre_policy_ref,
                bound_policies.ongoing_policy_ref,
                bound_policies.post_policy_ref
            FROM usage_sessions us

            LEFT JOIN LATERAL (
                SELECT
                    MAX(pr.policy_ref)
                        FILTER (WHERE pr.phase = 'PRE')
                        AS pre_policy_ref,

                    MAX(pr.policy_ref)
                        FILTER (WHERE pr.phase = 'ONGOING')
                        AS ongoing_policy_ref,

                    MAX(pr.policy_ref)
                        FILTER (WHERE pr.phase = 'POST')
                        AS post_policy_ref

                FROM usage_session_policies usp
                JOIN policy_registry pr
                ON pr.id = usp.policy_id

                WHERE usp.usage_session_id = us.id
            ) bound_policies ON TRUE

            WHERE us.id = ?
            """, sessionId);
    }

    public List<SessionDependency> findDependencies(Long sessionId) {
        return jdbcTemplate.query("""
            SELECT dependency_key,
                attr_key,
                entity_type,
                entity_id,
                attribute_name,
                monitor_type,
                next_check_at
            FROM usage_session_dependencies
            WHERE usage_session_id = ?
            AND active_flag = true
            ORDER BY id
        """, (rs, rowNum) -> new SessionDependency(
                rs.getString("dependency_key"),
                rs.getString("attr_key"),
                rs.getString("entity_type"),
                rs.getLong("entity_id"),
                rs.getString("attribute_name"),
                rs.getString("monitor_type"),
                rs.getTimestamp("next_check_at") == null
                        ? null
                        : rs.getTimestamp("next_check_at").toLocalDateTime()
        ), sessionId);
    }

    @Transactional
    public LocalDateTime revokeSession(
            Long sessionId,
            String reasonCode
    ) {
        LocalDateTime revokedAt = jdbcTemplate.queryForObject("""
            WITH transition_time AS (
                SELECT clock_timestamp() AS value
            )
            UPDATE usage_sessions us
            SET status = 'REVOKED',
                phase = 'POST',
                revoked_at = transition_time.value,
                ended_at = transition_time.value,
                revoked_reason = ?
            FROM transition_time
            WHERE us.id = ?
            AND us.status = 'ACTIVE'
            RETURNING us.revoked_at
            """,
            LocalDateTime.class,
            reasonCode,
            sessionId
        );

        // deactivateDependencies(sessionId);
        return revokedAt;
    }

    @Transactional
    public LocalDateTime endSession(Long sessionId) {
        LocalDateTime endedAt = jdbcTemplate.queryForObject("""
            WITH transition_time AS (
                SELECT clock_timestamp()::timestamp AS value
            )
            UPDATE usage_sessions us
            SET status = 'ENDED',
                phase = 'POST',
                ended_at = transition_time.value
            FROM transition_time
            WHERE us.id = ?
            AND us.status = 'ACTIVE'
            RETURNING us.ended_at
            """,
            LocalDateTime.class,
            sessionId
        );

        // deactivateDependencies(sessionId);
        return endedAt;
    }

    public boolean isPostProcessingCompleted(Long sessionId) {
        Boolean completed = jdbcTemplate.queryForObject("""
                SELECT post_handled_at IS NOT NULL
                FROM usage_sessions
                WHERE id = ?
                """, Boolean.class, sessionId);

        return Boolean.TRUE.equals(completed);
    }

    @Transactional
    public LocalDateTime completePostProcessing(Long sessionId) {
        LocalDateTime completedAt = jdbcTemplate.queryForObject("""
            WITH transition_time AS (
                SELECT clock_timestamp()::timestamp AS value
            )
            UPDATE usage_sessions us
            SET post_handled_at = transition_time.value
            FROM transition_time
            WHERE us.id = ?
            AND us.phase = 'POST'
            AND us.post_handled_at IS NULL
            RETURNING us.post_handled_at
            """,
            LocalDateTime.class,
            sessionId
        );

        deactivateDependencies(sessionId);

        return completedAt;
    }

    public List<Long> findAffectedActiveSessions(
            String entityType,
            Long entityId,
            String attributeName,
            MonitorType triggerMonitorType
    ) {
        return jdbcTemplate.queryForList("""
            SELECT DISTINCT us.id
            FROM usage_sessions us
            JOIN usage_session_dependencies dep
            ON dep.usage_session_id = us.id
            WHERE us.status = 'ACTIVE'
            AND dep.active_flag = true
            AND dep.entity_type = ?
            AND dep.entity_id = ?
            AND dep.attribute_name = ?
            AND dep.monitor_type IN (?, 'HYBRID')
        """, Long.class,
                entityType,
                entityId,
                attributeName,
                triggerMonitorType.name()
        );
    }

    private void deactivateDependencies(Long sessionId) {
        jdbcTemplate.update("""
            UPDATE usage_session_dependencies
            SET active_flag = false
            WHERE usage_session_id = ?
            AND active_flag = true
        """, sessionId);
    }
}