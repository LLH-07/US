package vn.ucon.demo.resource;

import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.sql.Timestamp;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
// import java.util.Locale;
import java.util.Map;

@Service
public class SubscriptionService {

    private final JdbcTemplate jdbcTemplate;

    public SubscriptionService(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    private List<Map<String, Object>> normalizePlanRows(List<Map<String, Object>> rows) {
        List<Map<String, Object>> normalized = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            Map<String, Object> safe = new LinkedHashMap<>(row);
            Long id = numberAsLong(safe.get("id"));
            safe.put("name", canonicalPlanName(id, String.valueOf(safe.getOrDefault("name", "G\u00f3i thu\u00ea"))));
            normalized.add(safe);
        }
        return normalized;
    }

    private Map<String, Object> normalizeRentalRow(Map<String, Object> row) {
        if (row == null) return null;
        Map<String, Object> safe = new LinkedHashMap<>(row);
        Long planId = numberAsLong(safe.get("plan_id"));
        safe.put("plan_name", canonicalPlanName(planId, String.valueOf(safe.getOrDefault("plan_name", "G\u00f3i thu\u00ea"))));
        return safe;
    }

    private Long numberAsLong(Object value) {
        return value instanceof Number number ? number.longValue() : null;
    }

    private String canonicalPlanName(Long id, String fallback) {
        if (id != null) {
            if (id == 1L) return "G\u00f3i th\u00e1ng";
            if (id == 2L) return "G\u00f3i n\u0103m";
        }
        return fallback == null || fallback.isBlank() ? "G\u00f3i thu\u00ea" : fallback;
    }

    private long nextId(String tableName) {
        String safeTable = tableName.replaceAll("[^a-zA-Z0-9_]", "");
        Long value = jdbcTemplate.queryForObject("SELECT COALESCE(MAX(id), 0) + 1 FROM " + safeTable, Long.class);
        return value == null ? 1L : value;
    }

    public Map<String, Object> subscriptionStatusForUser(Long userId) {
        List<Map<String, Object>> plans = jdbcTemplate.queryForList(
                """
                SELECT id, name, duration_days, price
                FROM rental_plans
                ORDER BY duration_days
                """
        );

        List<Map<String, Object>> rentals = jdbcTemplate.queryForList(
                """
                SELECT
                    r.id,
                    r.user_id,
                    r.plan_id,
                    p.name AS plan_name,
                    p.duration_days,
                    p.price,
                    to_char(r.started_at, 'YYYY-MM-DD HH24:MI:SS') AS started_at,
                    to_char(r.expires_at, 'YYYY-MM-DD HH24:MI:SS') AS expires_at,
                    r.status,
                    CASE
                        WHEN r.status = 'ACTIVE' AND r.expires_at > now() THEN 'ACTIVE'
                        WHEN r.status = 'ACTIVE' AND r.expires_at <= now() THEN 'EXPIRED'
                        ELSE r.status
                    END AS effective_status
                FROM user_rentals r
                JOIN rental_plans p ON p.id = r.plan_id
                WHERE r.user_id = ?
                ORDER BY r.expires_at DESC, r.id DESC
                LIMIT 1
                """,
                userId
        );

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("plans", normalizePlanRows(plans));
        result.put("currentRental", rentals.isEmpty() ? null : normalizeRentalRow(rentals.get(0)));
        return result;
    }

    public Map<String, Object> registerSubscription(Long userId, Long planId, Integer demoSeconds, Integer demoMinutes, Integer demoDays) {
        if (hasActiveRental(userId)) {
            throw new ResponseStatusException(HttpStatus.CONFLICT, "ACTIVE_SUBSCRIPTION_EXISTS");
        }

        Map<String, Object> plan = jdbcTemplate.queryForMap(
                """
                SELECT id, name, duration_days, price
                FROM rental_plans
                WHERE id = ?
                """,
                planId
        );

        long durationSeconds = resolveSubscriptionDurationSeconds(plan, demoSeconds, demoMinutes, demoDays);
        LocalDateTime startedAt = LocalDateTime.now();
        LocalDateTime expiresAt = startedAt.plusSeconds(durationSeconds);

        List<Long> rentalIds = jdbcTemplate.queryForList(
                """
                SELECT id
                FROM user_rentals
                WHERE user_id = ?
                ORDER BY
                    CASE WHEN status = 'ACTIVE' THEN 0 ELSE 1 END,
                    expires_at DESC,
                    id DESC
                LIMIT 1
                """,
                Long.class,
                userId
        );

        if (rentalIds.isEmpty()) {
            jdbcTemplate.update(
                    """
                    INSERT INTO user_rentals(id, user_id, plan_id, started_at, expires_at, status)
                    VALUES (?, ?, ?, ?, ?, 'ACTIVE')
                    """,
                    nextId("user_rentals"),
                    userId,
                    planId,
                    Timestamp.valueOf(startedAt),
                    Timestamp.valueOf(expiresAt)
            );
        } else {
            jdbcTemplate.update(
                    """
                    UPDATE user_rentals
                    SET plan_id = ?, started_at = ?, expires_at = ?, status = 'ACTIVE'
                    WHERE id = ?
                    """,
                    planId,
                    Timestamp.valueOf(startedAt),
                    Timestamp.valueOf(expiresAt),
                    rentalIds.get(0)
            );
        }

        Map<String, Object> result = subscriptionStatusForUser(userId);
        result.put("message", "SUBSCRIPTION_UPDATED");
        result.put("durationSeconds", durationSeconds);
        return result;
    }

    public Map<String, Object> cancelSubscription(Long userId) {
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "LOGIN_REQUIRED");
        }

        List<Long> activeIds = jdbcTemplate.queryForList(
                """
                SELECT id
                FROM user_rentals
                WHERE user_id = ?
                  AND status = 'ACTIVE'
                  AND expires_at > now()
                ORDER BY expires_at DESC, id DESC
                LIMIT 1
                """,
                Long.class,
                userId
        );

        if (activeIds.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "NO_ACTIVE_SUBSCRIPTION");
        }

        jdbcTemplate.update(
                """
                UPDATE user_rentals
                SET status = 'CANCELLED', expires_at = now()
                WHERE id = ?
                """,
                activeIds.get(0)
        );

        Map<String, Object> result = subscriptionStatusForUser(userId);
        result.put("message", "SUBSCRIPTION_CANCELLED");
        return result;
    }

    private long resolveSubscriptionDurationSeconds(Map<String, Object> plan, Integer demoSeconds, Integer demoMinutes, Integer demoDays) {
        if (demoSeconds != null && demoSeconds > 0) return Math.min(demoSeconds.longValue(), 31_536_000L);
        if (demoMinutes != null && demoMinutes > 0) return Math.min(demoMinutes.longValue() * 60L, 31_536_000L);
        if (demoDays != null && demoDays > 0) return Math.min(demoDays.longValue() * 86_400L, 31_536_000L);
        Number durationDays = (Number) plan.getOrDefault("duration_days", 30);
        return Math.max(1L, durationDays.longValue()) * 86_400L;
    }

    public boolean hasActiveRental(Long userId) {
        if (userId == null || userId <= 0) return false;
        Integer count = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM user_rentals
                WHERE user_id = ?
                  AND status = 'ACTIVE'
                  AND expires_at > now()
                """,
                Integer.class,
                userId
        );
        return count != null && count > 0;
    }
}
