package vn.ucon.demo.ucon.event;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.transaction.support.TransactionTemplate;
import vn.ucon.demo.ucon.ch.ContextHandler;
import vn.ucon.demo.ucon.model.TriggerEvent;

import java.util.List;

@Component
public class UconTriggerEventPoller {

    private final JdbcTemplate jdbcTemplate;
    private final TransactionTemplate transactionTemplate;
    private final ContextHandler contextHandler;

    public UconTriggerEventPoller(
            JdbcTemplate jdbcTemplate,
            ContextHandler contextHandler,
            TransactionTemplate transactionTemplate
    ) {
        this.jdbcTemplate = jdbcTemplate;
        this.contextHandler = contextHandler;
        this.transactionTemplate = transactionTemplate;
    }

    @Scheduled(fixedDelayString = "${ucon.trigger-poller.interval-ms:1000}")
    public void pollEvents() {
        processPendingEvents();
    }

    public int processPendingEvents() {
        List<TriggerEvent> events = jdbcTemplate.query("""
            WITH picked AS (
                SELECT id
                FROM ucon_trigger_events
                WHERE processed_flag = false
                ORDER BY occurred_at
                LIMIT 20
                FOR UPDATE SKIP LOCKED
            )
            SELECT e.*
            FROM ucon_trigger_events e
            JOIN picked p ON p.id = e.id
            ORDER BY e.occurred_at
        """, (rs, rowNum) -> new TriggerEvent(
                rs.getLong("id"),
                rs.getString("source_type"),
                rs.getString("entity_type"),
                rs.getLong("entity_id"),
                rs.getString("attribute_name"),
                rs.getString("event_type")
        ));

        for (TriggerEvent event : events) {
            transactionTemplate.executeWithoutResult(status -> processOneEvent(event));
        }

        return events.size();
    }

    private void processOneEvent(TriggerEvent event) {
        try {
            contextHandler.handleTrigger(event);

            jdbcTemplate.update("""
                UPDATE ucon_trigger_events
                SET processed_flag = true,
                    processed_at = now()
                WHERE id = ?
            """, event.id());
        } catch (Exception e) {
            jdbcTemplate.update("""
                UPDATE ucon_trigger_events
                SET processed_flag = true,
                    processed_at = now(),
                    payload_json = COALESCE(payload_json, '{}'::jsonb)
                        || jsonb_build_object(
                            'processing_error',
                            ?,
                            'processing_error_type',
                            ?
                        )
                WHERE id = ?
            """, e.getMessage(), e.getClass().getSimpleName(), event.id());

            System.err.println("[UCON POLLER] Failed to handle trigger event "
                    + event.id()
                    + ": "
                    + e.getClass().getSimpleName()
                    + " - "
                    + e.getMessage());
        }
    }
}