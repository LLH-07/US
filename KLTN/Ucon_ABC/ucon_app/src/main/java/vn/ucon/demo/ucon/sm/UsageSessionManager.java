package vn.ucon.demo.ucon.sm;

import java.util.List;
import java.util.Map;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.MonitorType;
import vn.ucon.demo.ucon.model.PolicyDecision;
import vn.ucon.demo.ucon.model.PolicyMetadata;
import vn.ucon.demo.ucon.model.SessionDependency;

@Component
public class UsageSessionManager {

    private final JdbcTemplate jdbcTemplate;

    public UsageSessionManager(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Transactional
    public Long createPendingSession(
            AccessRequest request,
            List<PolicyMetadata> boundPolicies,
            List<SessionDependency> dependencies,
            PolicyDecision decision
    ) {
        Long sessionId = jdbcTemplate.queryForObject("""
            INSERT INTO usage_sessions(
                subject_user_id,
                resource_type,
                resource_id,
                action_id,
                access_path,
                phase,
                status
            )
            VALUES (?, ?, ?, ?, ?, 'PRE', 'PENDING')
            RETURNING id
        """, Long.class,
                request.subjectUserId(),
                request.resourceType(),
                request.resourceId(),
                request.actionId(),
                request.accessPath()
        );

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

        logEvent(
                sessionId,
                "PRE_PERMIT",
                "PRE",
                decision.rawDecision(),
                decision.reasonCode(),
                "{\"message\":\"pending session created after PRE permit\"}"
        );

        return sessionId;
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
    public boolean activateSession(Long sessionId) {
        int updated = jdbcTemplate.update("""
            UPDATE usage_sessions
            SET status = 'ACTIVE',
                phase = 'ONGOING',
                activated_at = now(),
                last_eval_at = now()
            WHERE id = ?
              AND status = 'PENDING'
        """, sessionId);

        if (updated > 0) {
            logEvent(
                    sessionId,
                    "START_ACCESS",
                    "ONGOING",
                    "PERMIT",
                    "STARTED",
                    "{\"message\":\"session activated when resource usage starts\"}"
            );
        }

        return updated > 0;
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
    public void denyPendingSession(Long sessionId, String reasonCode) {
        jdbcTemplate.update("""
            UPDATE usage_sessions
            SET status = 'DENIED',
                phase = 'POST',
                ended_at = now(),
                revoked_reason = ?,
                post_handled_at = now()
            WHERE id = ?
              AND status = 'PENDING'
        """, reasonCode, sessionId);

        logEvent(
                sessionId,
                "DENY_START_ACCESS",
                "ONGOING",
                "DENY",
                reasonCode,
                "{\"message\":\"pending session denied before actual resource usage\"}"
        );
    }

    public Map<String, Object> findSession(Long sessionId) {
        return jdbcTemplate.queryForMap("""
            SELECT *
            FROM usage_sessions
            WHERE id = ?
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
    public void revokeSession(Long sessionId, String reasonCode) {
        jdbcTemplate.update("""
            UPDATE usage_sessions
            SET status = 'REVOKED',
                phase = 'POST',
                revoked_at = now(),
                ended_at = now(),
                revoked_reason = ?
            WHERE id = ?
              AND status = 'ACTIVE'
        """, reasonCode, sessionId);

        deactivateDependencies(sessionId);

        logEvent(
                sessionId,
                "REVOKE_ACCESS",
                "ONGOING",
                "DENY",
                reasonCode,
                "{\"message\":\"session revoked after ongoing policy became unsatisfied\"}"
        );
    }

    @Transactional
    public void endSession(Long sessionId) {
        jdbcTemplate.update("""
            UPDATE usage_sessions
            SET status = 'ENDED',
                phase = 'POST',
                ended_at = now()
            WHERE id = ?
            AND status = 'ACTIVE'
        """, sessionId);

        deactivateDependencies(sessionId);

        logEvent(
                sessionId,
                "END_ACCESS",
                "POST",
                "PERMIT",
                "USER_END",
                "{\"message\":\"normal endAccess\"}"
        );
    }

    public boolean isPostHandled(Long sessionId) {
        Boolean handled = jdbcTemplate.queryForObject("""
                SELECT post_handled_at IS NOT NULL
                FROM usage_sessions
                WHERE id = ?
                """, Boolean.class, sessionId);

        return Boolean.TRUE.equals(handled);
    }

    @Transactional
    public void markPostHandled(Long sessionId) {
        jdbcTemplate.update("""
            UPDATE usage_sessions
            SET post_handled_at = now()
            WHERE id = ?
            AND phase = 'POST'
        """, sessionId);

        logEvent(
                sessionId,
                "POST_HANDLED",
                "POST",
                "PERMIT",
                "POST_UPDATE_DONE",
                "{\"message\":\"post update instructions handled\"}"
        );
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

    public void logEvent(
            Long sessionId,
            String eventType,
            String phase,
            String decision,
            String reasonCode,
            String detailsJson
    ) {
        jdbcTemplate.update("""
            INSERT INTO session_events(
                usage_session_id,
                event_type,
                phase,
                decision,
                reason_code,
                details_json
            )
            VALUES (?, ?, ?, ?, ?, ?::jsonb)
        """, sessionId, eventType, phase, decision, reasonCode, detailsJson);
    }
}