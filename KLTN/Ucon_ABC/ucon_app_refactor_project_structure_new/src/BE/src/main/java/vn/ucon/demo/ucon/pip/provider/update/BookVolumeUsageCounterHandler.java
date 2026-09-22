package vn.ucon.demo.ucon.pip.provider.update;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;
import vn.ucon.demo.ucon.pip.provider.AttributeUpdateHandler;
import vn.ucon.demo.ucon.pip.provider.ResolvedAttributeUpdate;

@Component
public class BookVolumeUsageCounterHandler implements AttributeUpdateHandler {

    private final JdbcTemplate jdbcTemplate;

    public BookVolumeUsageCounterHandler(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String handlerName) {
        return "BookVolumeUsageCounterHandler".equals(handlerName);
    }

    @Override
    @Transactional
    public void apply(ResolvedAttributeUpdate update) {
        switch (update.operation()) {
            case "INCREMENT" -> incrementReadCount(update);
            case "DECREMENT" -> decrementReadCount(update);
            default -> throw new IllegalArgumentException(
                    "Unsupported book volume counter operation: " + update.operation()
            );
        }
    }

    private void incrementReadCount(ResolvedAttributeUpdate update) {
        long amount = toLong(update.value());

        int updated = jdbcTemplate.update("""
            UPDATE book_volumes bv
            SET read_count = COALESCE(bv.read_count, 0) + ?
            WHERE bv.id = (
                SELECT us.resource_id
                FROM usage_sessions us
                WHERE us.id = ?
                  AND us.resource_type = 'BOOK_VOLUME'
            )
        """, amount, update.usageSessionId());

        ensureUpdated(updated, update);
    }

    private void decrementReadCount(ResolvedAttributeUpdate update) {
        long amount = toLong(update.value());

        int updated = jdbcTemplate.update("""
            UPDATE book_volumes bv
            SET read_count = GREATEST(COALESCE(bv.read_count, 0) - ?, 0)
            WHERE bv.id = (
                SELECT us.resource_id
                FROM usage_sessions us
                WHERE us.id = ?
                  AND us.resource_type = 'BOOK_VOLUME'
            )
        """, amount, update.usageSessionId());

        ensureUpdated(updated, update);
    }

    private void ensureUpdated(int updated, ResolvedAttributeUpdate update) {
        if (updated == 0) {
            throw new IllegalStateException(
                    "Cannot update book volume read_count for actionCode="
                            + update.actionCode()
                            + ", sessionId="
                            + update.usageSessionId()
            );
        }
    }

    private long toLong(Object value) {
        if (value == null) {
            return 1L;
        }

        if (value instanceof Number number) {
            return number.longValue();
        }

        return Long.parseLong(value.toString());
    }
}