package vn.ucon.demo.ucon.event;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import org.springframework.transaction.support.TransactionTemplate;
import vn.ucon.demo.ucon.ch.ContextHandler;
import vn.ucon.demo.ucon.model.TriggerEvent;

import java.util.List;

@Component
public class UconTriggerEventPoller {

    private final JdbcTemplate jdbcTemplate;
    private final TransactionTemplate transactionTemplate;
    private final ContextHandler contextHandler;

    private final Object metricsLock = new Object();

    private long metricsStartedAtNanos = System.nanoTime();
    private long pollRuns = 0;
    private long emptyPolls = 0;
    private long processedEvents = 0;

    public UconTriggerEventPoller(
            JdbcTemplate jdbcTemplate,
            ContextHandler contextHandler,
            TransactionTemplate transactionTemplate
    ) {
        this.jdbcTemplate = jdbcTemplate;
        this.contextHandler = contextHandler;
        this.transactionTemplate = transactionTemplate;
    }

    @Scheduled(fixedDelayString = "${ucon.trigger-poller.interval-ms}")
    public void pollEvents() {
        int eventCount = processPendingEvents();
        recordPoll(eventCount);
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
            markPicked(event);

            contextHandler.handleTrigger(event);

            jdbcTemplate.update("""
                UPDATE ucon_trigger_events
                SET processed_flag = true,
                    processed_at = clock_timestamp()
                WHERE id = ?
            """, event.id());
        } catch (Exception e) {
            jdbcTemplate.update("""
                UPDATE ucon_trigger_events
                SET processed_flag = true,
                    processed_at = clock_timestamp(),
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

    private void markPicked(TriggerEvent event) {
        jdbcTemplate.update("""
            UPDATE ucon_trigger_events
            SET picked_at = COALESCE(picked_at, clock_timestamp())
            WHERE id = ?
        """, event.id());
    }

    private void recordPoll(int eventCount) {
        synchronized (metricsLock) {
            pollRuns++;

            if (eventCount == 0) {
                emptyPolls++;
            }

            processedEvents += eventCount;
        }
    }

    public void resetMetrics() {
        synchronized (metricsLock) {
            metricsStartedAtNanos = System.nanoTime();
            pollRuns = 0;
            emptyPolls = 0;
            processedEvents = 0;
        }
    }

    public PollerMetrics snapshotMetrics() {
        synchronized (metricsLock) {
            long elapsedNanos =
                    System.nanoTime() - metricsStartedAtNanos;

            long elapsedMs =
                    elapsedNanos / 1_000_000;

            double elapsedMinutes =
                    elapsedNanos / 60_000_000_000.0;

            double runsPerMinute =
                    elapsedMinutes > 0
                            ? pollRuns / elapsedMinutes
                            : 0.0;

            double emptyPerMinute =
                    elapsedMinutes > 0
                            ? emptyPolls / elapsedMinutes
                            : 0.0;

            double eventsPerPoll =
                    pollRuns > 0
                            ? (double) processedEvents / pollRuns
                            : 0.0;

            return new PollerMetrics(
                    elapsedMs,
                    pollRuns,
                    emptyPolls,
                    processedEvents,
                    runsPerMinute,
                    emptyPerMinute,
                    eventsPerPoll
            );
        }
    }
    public record PollerMetrics(
            long elapsedMs,
            long pollRuns,
            long emptyPolls,
            long eventsProcessed,
            double pollerRunsPerMinute,
            double emptyPollsPerMinute,
            double eventsProcessedPerPoll
    ) {
    }
}