package vn.ucon.demo.ucon.event;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.List;

@Component
public class UconTimerDependencyScheduler {

    private final JdbcTemplate jdbcTemplate;

    public UconTimerDependencyScheduler(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Scheduled(fixedDelay = 1000)
    public void detectDueTimerDependencies() {
        List<DueDependency> dueDependencies = jdbcTemplate.query("""
            SELECT DISTINCT
                   dep.usage_session_id,
                   dep.entity_type,
                   dep.entity_id,
                   dep.attribute_name
            FROM usage_session_dependencies dep
            JOIN usage_sessions us
              ON us.id = dep.usage_session_id
            WHERE us.status = 'ACTIVE'
              AND dep.active_flag = true
              AND dep.monitor_type IN ('TIMER', 'HYBRID')
              AND dep.next_check_at IS NOT NULL
              AND dep.next_check_at <= now()
            LIMIT 100
        """, (rs, rowNum) -> new DueDependency(
                rs.getLong("usage_session_id"),
                rs.getString("entity_type"),
                rs.getLong("entity_id"),
                rs.getString("attribute_name")
        ));

        for (DueDependency dep : dueDependencies) {
            insertTimerEvent(dep);
            clearNextCheckAt(dep);
        }
    }

    private void insertTimerEvent(DueDependency dep) {
        jdbcTemplate.update("""
            INSERT INTO ucon_trigger_events(
                source_type,
                entity_type,
                entity_id,
                attribute_name,
                event_type,
                payload_json
            )
            VALUES (
                'SCHEDULER',
                ?,
                ?,
                ?,
                'TIMER_DUE',
                jsonb_build_object(
                    'usage_session_id', ?,
                    'message', 'timer dependency is due'
                )
            )
        """,
                dep.entityType(),
                dep.entityId(),
                dep.attributeName(),
                dep.usageSessionId()
        );
    }

    private void clearNextCheckAt(DueDependency dep) {
        jdbcTemplate.update("""
            UPDATE usage_session_dependencies
            SET next_check_at = NULL
            WHERE usage_session_id = ?
              AND entity_type = ?
              AND entity_id = ?
              AND attribute_name = ?
              AND monitor_type IN ('TIMER', 'HYBRID')
        """,
                dep.usageSessionId(),
                dep.entityType(),
                dep.entityId(),
                dep.attributeName()
        );
    }

    private record DueDependency(
            Long usageSessionId,
            String entityType,
            Long entityId,
            String attributeName
    ) {
    }
}