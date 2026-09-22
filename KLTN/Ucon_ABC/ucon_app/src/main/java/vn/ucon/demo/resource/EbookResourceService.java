package vn.ucon.demo.resource;

import org.springframework.core.io.Resource;
import org.springframework.core.io.ResourceLoader;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.File;
import java.io.InputStream;
import java.sql.Timestamp;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.HashSet;
import java.util.Set;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.time.LocalDateTime;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

@Service
public class EbookResourceService {

    private static final String DEFAULT_EPUB_PATH = "";
    private static final int DEFAULT_CHUNK_SIZE = 3400;
    private static final int DEFAULT_PAGES_PER_VOLUME = 20;

    private final JdbcTemplate jdbcTemplate;
    private final ResourceLoader resourceLoader;
    private final Map<String, List<String>> serverTextCache = new ConcurrentHashMap<>();
    private final Map<String, EpubMeta> epubMetaCache = new ConcurrentHashMap<>();
    private final Map<String, ContentAvailability> contentAvailabilityCache = new ConcurrentHashMap<>();

    public EbookResourceService(JdbcTemplate jdbcTemplate, ResourceLoader resourceLoader) {
        this.jdbcTemplate = jdbcTemplate;
        this.resourceLoader = resourceLoader;
    }

    private List<Map<String, Object>> normalizeUserRows(List<Map<String, Object>> rows) {
        List<Map<String, Object>> normalized = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            Map<String, Object> safe = new LinkedHashMap<>(row);
            Long id = numberAsLong(safe.get("id"));
            String email = String.valueOf(safe.getOrDefault("email", ""));
            safe.put("name", canonicalUserName(id, email, String.valueOf(safe.getOrDefault("name", "Ng\u01b0\u1eddi d\u00f9ng"))));
            normalized.add(safe);
        }
        return normalized;
    }

    private List<Map<String, Object>> normalizeGroupRows(List<Map<String, Object>> rows) {
        List<Map<String, Object>> normalized = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            Map<String, Object> safe = new LinkedHashMap<>(row);
            Long id = numberAsLong(safe.get("id"));
            safe.put("name", canonicalGroupName(id, String.valueOf(safe.getOrDefault("name", "Nh\u00f3m gia \u0111\u00ecnh"))));
            normalized.add(safe);
        }
        return normalized;
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

    private int numberAsInt(Object value, int fallback) {
        if (value instanceof Number number) return number.intValue();
        try {
            return value == null ? fallback : Integer.parseInt(String.valueOf(value));
        } catch (Exception ex) {
            return fallback;
        }
    }

    private String canonicalUserName(Long id, String email, String fallback) {
        if (id != null) {
            if (id == 1L) return "Nguy\u1ec5n Th\u1ecb B\u00ecnh";
            if (id == 2L) return "Tr\u1ea7n Minh An";
            if (id == 3L) return "L\u00ea B\u1ea3o Ch\u00e2u";
            if (id == 4L) return "H\u00e0 Linh";
            if (id == 5L) return "Phan Thanh T\u00f9ng";
            if (id == 6L) return "\u0110\u1ed7 Gia H\u00e2n";
            if (id == 7L) return "Ph\u1ea1m Ho\u00e0ng Nam";
            if (id == 8L) return "V\u00f5 Minh Khang";
        }
        String safeEmail = email == null ? "" : email.toLowerCase(Locale.ROOT);
        if (safeEmail.equals("nguyenbinh123@gmail.com")) return "Nguy\u1ec5n Th\u1ecb B\u00ecnh";
        if (safeEmail.equals("an.demo@gmail.com")) return "Tr\u1ea7n Minh An";
        if (safeEmail.equals("chau.demo@gmail.com")) return "L\u00ea B\u1ea3o Ch\u00e2u";
        if (safeEmail.equals("halinh.demo@gmail.com")) return "H\u00e0 Linh";
        if (safeEmail.equals("ptt.demo@gmail.com")) return "Phan Thanh T\u00f9ng";
        if (safeEmail.equals("han.demo@gmail.com")) return "\u0110\u1ed7 Gia H\u00e2n";
        if (safeEmail.equals("nam.demo@gmail.com")) return "Ph\u1ea1m Ho\u00e0ng Nam";
        if (safeEmail.equals("khang.demo@gmail.com")) return "V\u00f5 Minh Khang";
        return fallback == null || fallback.isBlank() ? "Ng\u01b0\u1eddi d\u00f9ng" : fallback;
    }

    private String canonicalGroupName(Long id, String fallback) {
        if (id != null) {
            if (id == 1L) return "Gia \u0111\u00ecnh nh\u00e0 B\u00ecnh";
            if (id == 2L) return "Nh\u00f3m tri th\u1ee9c";
            if (id == 3L) return "T\u1ee7 s\u00e1ch cu\u1ed1i tu\u1ea7n";
            if (id == 4L) return "C\u00e2u l\u1ea1c b\u1ed9 \u0111\u1ecdc s\u00e1ch";
            if (id == 5L) return "Kho s\u00e1ch h\u1ecdc t\u1eadp";
        }
        return fallback == null || fallback.isBlank() ? "Nh\u00f3m gia \u0111\u00ecnh" : fallback;
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

    public boolean hasPurchasedBook(Long userId, Long bookId) {
        // Trong bản này, "mua sách" được hiểu là user đã mua ÍT NHẤT MỘT tập của sách.
        // Quyền đọc đầy đủ vẫn kiểm tra theo từng tập bằng hasPurchasedVolume(...).
        return !purchasedReadablePaidVolumeIdsForBook(userId, bookId).isEmpty();
    }

    private List<Long> purchasedReadablePaidVolumeIdsForBook(Long userId, Long bookId) {
        if (userId == null || userId <= 0 || bookId == null) return List.of();
        List<Long> readablePaidVolumes = readablePaidVolumeIdsForBook(bookId);
        if (readablePaidVolumes.isEmpty()) return List.of();

        String placeholders = String.join(",", readablePaidVolumes.stream().map(id -> "?").toList());
        List<Object> args = new ArrayList<>();
        args.add(userId);
        args.addAll(readablePaidVolumes);

        return jdbcTemplate.queryForList(
                ("""
                SELECT DISTINCT vp.volume_id
                FROM volume_purchases vp
                WHERE vp.user_id = ?
                  AND vp.volume_id IN (%s)
                ORDER BY vp.volume_id
                """.formatted(placeholders)),
                Long.class,
                args.toArray()
        );
    }


    public boolean hasPurchasedVolume(Long userId, Long volumeId) {
        if (userId == null || userId <= 0 || volumeId == null) return false;
        Integer count = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM volume_purchases vp
                JOIN book_volumes bv ON bv.id = vp.volume_id
                JOIN books b ON b.id = bv.book_id
                WHERE vp.user_id = ?
                  AND vp.volume_id = ?
                  AND COALESCE(bv.access_model_override, b.default_access_model) = 'PAID'
                """,
                Integer.class,
                userId,
                volumeId
        );
        return count != null && count > 0;
    }

    public boolean hasSharedVolumeAccess(Long userId, Long volumeId) {
        if (userId == null || userId <= 0 || volumeId == null) return false;
        Integer count = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM family_shared_volumes fsv
                JOIN family_groups g ON g.id = fsv.group_id
                JOIN volume_purchases owner_vp
                    ON owner_vp.user_id = g.owner_user_id
                   AND owner_vp.volume_id = fsv.volume_id
                LEFT JOIN family_group_members m
                    ON m.group_id = g.id
                   AND m.user_id = ?
                   AND m.status = 'ACTIVE'
                WHERE fsv.volume_id = ?
                  AND fsv.status = 'ACTIVE'
                  AND g.status = 'ACTIVE'
                  AND (g.owner_user_id = ? OR m.user_id IS NOT NULL)
                """,
                Integer.class,
                userId,
                volumeId,
                userId
        );
        return count != null && count > 0;
    }

    public boolean hasSharedBookAccess(Long userId, Long bookId) {
        if (userId == null || userId <= 0 || bookId == null) return false;
        Integer count = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM family_shared_volumes fsv
                JOIN book_volumes bv ON bv.id = fsv.volume_id
                JOIN family_groups g ON g.id = fsv.group_id
                JOIN volume_purchases owner_vp
                    ON owner_vp.user_id = g.owner_user_id
                   AND owner_vp.volume_id = fsv.volume_id
                LEFT JOIN family_group_members m
                    ON m.group_id = g.id
                   AND m.user_id = ?
                   AND m.status = 'ACTIVE'
                WHERE bv.book_id = ?
                  AND bv.status = 'ACTIVE'
                  AND fsv.status = 'ACTIVE'
                  AND g.status = 'ACTIVE'
                  AND (g.owner_user_id = ? OR m.user_id IS NOT NULL)
                """,
                Integer.class,
                userId,
                bookId,
                userId
        );
        return count != null && count > 0;
    }

    public List<Map<String, Object>> purchaseLibraryForUser(Long userId) {
        List<Map<String, Object>> rows = jdbcTemplate.queryForList(
                """
                WITH ranked AS (
                    SELECT
                        b.id AS book_id,
                        b.title AS book_title,
                        b.genre,
                        b.authors,
                        b.default_access_model,
                        MIN(vp.purchased_at) AS first_purchased_at,
                        MAX(vp.purchased_at) AS last_purchased_at,
                        COUNT(DISTINCT vp.volume_id) AS purchased_volume_count,
                        STRING_AGG(DISTINCT vp.volume_id::text, ',') AS purchased_volume_ids
                    FROM volume_purchases vp
                    JOIN book_volumes bv ON bv.id = vp.volume_id
                    JOIN books b ON b.id = bv.book_id
                    WHERE vp.user_id = ?
                      AND b.status = 'ACTIVE'
                      AND bv.status = 'ACTIVE'
                      AND COALESCE(bv.access_model_override, b.default_access_model) = 'PAID'
                    GROUP BY b.id, b.title, b.genre, b.authors, b.default_access_model
                )
                SELECT
                    book_id,
                    book_title,
                    genre,
                    authors,
                    default_access_model,
                    purchased_volume_count,
                    purchased_volume_ids,
                    to_char(first_purchased_at, 'YYYY-MM-DD HH24:MI:SS') AS first_purchased_at,
                    to_char(last_purchased_at, 'YYYY-MM-DD HH24:MI:SS') AS last_purchased_at
                FROM ranked
                ORDER BY last_purchased_at DESC, book_id
                """,
                userId
        );
        return readablePurchasedLibraryRows(rows);
    }

    public Map<String, Object> purchaseBook(Long userId, Long bookId) {
        // Mua toàn bộ sách trả phí: tạo quyền mua cho tất cả tập PAID còn active của sách.
        // Vẫn dùng bảng volume_purchases để không đổi schema và không ảnh hưởng logic UCON theo từng tập.
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "LOGIN_REQUIRED");
        }
        Integer bookCount = jdbcTemplate.queryForObject(
                "SELECT COUNT(*) FROM books WHERE id = ? AND status = 'ACTIVE'",
                Integer.class,
                bookId
        );
        if (bookCount == null || bookCount == 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_NOT_FOUND");
        }
        String bookAccessModel = effectiveAccessModelForBook(bookId);
        if (!"PAID".equals(bookAccessModel)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_PAID_BOOK_CAN_BE_PURCHASED");
        }

        List<Map<String, Object>> volumes = readablePaidVolumeRowsForBook(bookId);
        if (volumes.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_HAS_NO_ACTIVE_PAID_VOLUME");
        }

        Set<Long> purchasedIds = new HashSet<>(purchasedReadablePaidVolumeIdsForBook(userId, bookId));
        int inserted = 0;

        for (Map<String, Object> volume : volumes) {
            Long volumeId = numberAsLong(volume.get("id"));
            if (purchasedIds.contains(volumeId)) {
                continue;
            }
            Number price = (Number) volume.getOrDefault("price", 0);
            jdbcTemplate.update(
                    "INSERT INTO volume_purchases(id, user_id, volume_id, price, purchased_at) VALUES (?, ?, ?, ?, now())",
                    nextId("volume_purchases"),
                    userId,
                    volumeId,
                    price
            );
            inserted++;
        }

        List<Long> currentPurchasedIds = purchasedReadablePaidVolumeIdsForBook(userId, bookId);
        Map<String, Object> result = new LinkedHashMap<>();
        result.put("message", "PURCHASED_BOOK");
        result.put("bookId", bookId);
        result.put("newlyPurchasedVolumeCount", inserted);
        result.put("purchasedVolumeCount", currentPurchasedIds.size());
        result.put("purchasedVolumeIds", currentPurchasedIds);
        result.put("library", purchaseLibraryForUser(userId));
        return result;
    }

    public Map<String, Object> purchaseVolume(Long userId, Long volumeId) {
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "LOGIN_REQUIRED");
        }
        if (volumeId == null || volumeId <= 0) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "VOLUME_REQUIRED");
        }

        Map<String, Object> volume = paidReadableVolumeRow(volumeId);
        Long bookId = numberAsLong(volume.get("book_id"));
        Number price = (Number) volume.getOrDefault("price", 0);

        Integer exists = jdbcTemplate.queryForObject(
                "SELECT COUNT(*) FROM volume_purchases WHERE user_id = ? AND volume_id = ?",
                Integer.class,
                userId,
                volumeId
        );
        if (exists == null || exists == 0) {
            jdbcTemplate.update(
                    "INSERT INTO volume_purchases(id, user_id, volume_id, price, purchased_at) VALUES (?, ?, ?, ?, now())",
                    nextId("volume_purchases"),
                    userId,
                    volumeId,
                    price
            );
        }

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("message", "PURCHASED_VOLUME");
        result.put("bookId", bookId);
        result.put("volumeId", volumeId);
        result.put("volumeNo", volume.get("volume_no"));
        result.put("library", purchaseLibraryForUser(userId));
        return result;
    }

    private Map<String, Object> paidReadableVolumeRow(Long volumeId) {
        Map<String, Object> volume;
        try {
            volume = jdbcTemplate.queryForMap(
                    """
                    SELECT bv.id,
                           bv.book_id,
                           bv.volume_no,
                           COALESCE(bv.price, 0) AS price,
                           COALESCE(bv.access_model_override, b.default_access_model) AS access_model
                    FROM book_volumes bv
                    JOIN books b ON b.id = bv.book_id
                    WHERE bv.id = ?
                      AND bv.status = 'ACTIVE'
                      AND b.status = 'ACTIVE'
                    """,
                    volumeId
            );
        } catch (EmptyResultDataAccessException ex) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "VOLUME_NOT_FOUND");
        }

        String model = String.valueOf(volume.getOrDefault("access_model", "FREE")).toUpperCase(Locale.ROOT);
        if (!"PAID".equals(model)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_PAID_VOLUME_CAN_BE_PURCHASED");
        }

        VolumeContentMetadata metadata = metadataForVolume(volumeId);
        if (!metadata.contentAvailable() || metadata.totalChunks() <= 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_CONTENT_NOT_AVAILABLE");
        }
        return volume;
    }

    public Map<String, Object> familyDataForUser(Long userId) {
        List<Map<String, Object>> groups = jdbcTemplate.queryForList(
                """
                SELECT DISTINCT g.id, g.owner_user_id, g.name, g.status
                FROM family_groups g
                LEFT JOIN family_group_members m
                    ON m.group_id = g.id
                   AND m.status = 'ACTIVE'
                WHERE g.status = 'ACTIVE'
                  AND (g.owner_user_id = ? OR m.user_id = ?)
                ORDER BY g.id
                """,
                userId,
                userId
        );

        List<Long> groupIds = groups.stream()
                .map(g -> ((Number) g.get("id")).longValue())
                .toList();

        List<Map<String, Object>> members = new ArrayList<>();
        List<Map<String, Object>> sharedVolumes = new ArrayList<>();
        if (!groupIds.isEmpty()) {
            String placeholders = String.join(",", groupIds.stream().map(id -> "?").toList());
            Object[] args = groupIds.toArray();

            members = jdbcTemplate.queryForList(
                    """
                    SELECT id, group_id, user_id, status,
                           to_char(joined_at, 'YYYY-MM-DD HH24:MI:SS') AS joined_at,
                           to_char(removed_at, 'YYYY-MM-DD HH24:MI:SS') AS removed_at
                    FROM family_group_members
                    WHERE group_id IN (%s)
                      AND status = 'ACTIVE'
                    ORDER BY group_id, id
                    """.formatted(placeholders),
                    args
            );

            sharedVolumes = jdbcTemplate.queryForList(
                    """
                    SELECT fsv.id, fsv.group_id, fsv.volume_id, fsv.status,
                           bv.book_id, b.title AS book_title, b.authors AS authors, bv.volume_no,
                           to_char(fsv.shared_at, 'YYYY-MM-DD HH24:MI:SS') AS shared_at,
                           to_char(fsv.revoked_at, 'YYYY-MM-DD HH24:MI:SS') AS revoked_at
                    FROM family_shared_volumes fsv
                    JOIN book_volumes bv ON bv.id = fsv.volume_id
                    JOIN books b ON b.id = bv.book_id
                    JOIN family_groups g ON g.id = fsv.group_id
                    JOIN volume_purchases owner_vp
                        ON owner_vp.user_id = g.owner_user_id
                       AND owner_vp.volume_id = fsv.volume_id
                    WHERE fsv.group_id IN (%s)
                      AND fsv.status = 'ACTIVE'
                      AND bv.status = 'ACTIVE'
                      AND b.status = 'ACTIVE'
                      AND COALESCE(bv.access_model_override, b.default_access_model) = 'PAID'
                    ORDER BY fsv.group_id, b.id, bv.volume_no
                    """.formatted(placeholders),
                    args
            );
        }

        List<Map<String, Object>> users = jdbcTemplate.queryForList(
                """
                SELECT id, name, email, status
                FROM users
                ORDER BY id
                """
        );

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("groups", normalizeGroupRows(groups));
        result.put("members", members);
        result.put("sharedVolumes", sharedVolumes);
        result.put("users", normalizeUserRows(users));
        return result;
    }


    public Map<String, Object> addFamilyMember(Long groupId, Long ownerUserId, Long memberUserId, String memberEmail) {
        ensureFamilyOwner(groupId, ownerUserId);
        Long resolvedMemberUserId = resolveMemberUserId(memberUserId, memberEmail);
        if (ownerUserId.equals(resolvedMemberUserId)) {
            return familyDataForUser(ownerUserId);
        }

        List<Long> existingIds = jdbcTemplate.queryForList(
                """
                SELECT id
                FROM family_group_members
                WHERE group_id = ? AND user_id = ?
                ORDER BY id DESC
                LIMIT 1
                """,
                Long.class,
                groupId,
                resolvedMemberUserId
        );

        if (existingIds.isEmpty()) {
            jdbcTemplate.update(
                    """
                    INSERT INTO family_group_members(id, group_id, user_id, status, joined_at, removed_at)
                    VALUES (?, ?, ?, 'ACTIVE', now(), NULL)
                    """,
                    nextId("family_group_members"),
                    groupId,
                    resolvedMemberUserId
            );
        } else {
            jdbcTemplate.update(
                    """
                    UPDATE family_group_members
                    SET status = 'ACTIVE', joined_at = now(), removed_at = NULL
                    WHERE id = ?
                    """,
                    existingIds.get(0)
            );
        }

        return familyDataForUser(ownerUserId);
    }

    private Long resolveMemberUserId(Long memberUserId, String memberEmail) {
        if (memberUserId != null && memberUserId > 0) return memberUserId;
        String safeEmail = memberEmail == null ? "" : memberEmail.trim().toLowerCase(Locale.ROOT);
        if (safeEmail.isBlank()) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "MEMBER_EMAIL_REQUIRED");
        }
        List<Long> ids = jdbcTemplate.queryForList(
                """
                SELECT id
                FROM users
                WHERE LOWER(email) = ?
                  AND status = 'ACTIVE'
                ORDER BY id
                LIMIT 1
                """,
                Long.class,
                safeEmail
        );
        if (ids.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "MEMBER_EMAIL_NOT_FOUND");
        }
        return ids.get(0);
    }

    public Map<String, Object> removeFamilyMember(Long groupId, Long ownerUserId, Long memberUserId) {
        ensureFamilyOwner(groupId, ownerUserId);
        if (ownerUserId.equals(memberUserId)) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "OWNER_CANNOT_BE_REMOVED");
        }

        jdbcTemplate.update(
                """
                UPDATE family_group_members
                SET status = 'REMOVED', removed_at = now()
                WHERE group_id = ? AND user_id = ? AND status = 'ACTIVE'
                """,
                groupId,
                memberUserId
        );

        return familyDataForUser(ownerUserId);
    }

    public Map<String, Object> leaveFamilyGroup(Long groupId, Long userId) {
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "LOGIN_REQUIRED");
        }

        Integer ownerCount = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM family_groups
                WHERE id = ? AND owner_user_id = ? AND status = 'ACTIVE'
                """,
                Integer.class,
                groupId,
                userId
        );
        if (ownerCount != null && ownerCount > 0) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "OWNER_CANNOT_LEAVE_USE_DISSOLVE");
        }

        int updated = jdbcTemplate.update(
                """
                UPDATE family_group_members
                SET status = 'REMOVED', removed_at = now()
                WHERE group_id = ? AND user_id = ? AND status = 'ACTIVE'
                """,
                groupId,
                userId
        );
        if (updated == 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "ACTIVE_MEMBERSHIP_NOT_FOUND");
        }

        return familyDataForUser(userId);
    }

    public Map<String, Object> dissolveFamilyGroup(Long groupId, Long ownerUserId) {
        ensureFamilyOwner(groupId, ownerUserId);

        jdbcTemplate.update(
                """
                UPDATE family_groups
                SET status = 'DISBANDED'
                WHERE id = ? AND owner_user_id = ? AND status = 'ACTIVE'
                """,
                groupId,
                ownerUserId
        );
        jdbcTemplate.update(
                """
                UPDATE family_group_members
                SET status = 'REMOVED', removed_at = now()
                WHERE group_id = ? AND status = 'ACTIVE'
                """,
                groupId
        );
        jdbcTemplate.update(
                """
                UPDATE family_shared_volumes
                SET status = 'REVOKED', revoked_at = now()
                WHERE group_id = ? AND status = 'ACTIVE'
                """,
                groupId
        );

        return familyDataForUser(ownerUserId);
    }


    private String effectiveAccessModelForBook(Long bookId) {
        try {
            String model = jdbcTemplate.queryForObject(
                    "SELECT default_access_model FROM books WHERE id = ? AND status = 'ACTIVE'",
                    String.class,
                    bookId
            );
            return model == null || model.isBlank() ? "FREE" : model.trim().toUpperCase(Locale.ROOT);
        } catch (EmptyResultDataAccessException ex) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_NOT_FOUND");
        }
    }

    public Map<String, Object> shareFamilyBook(Long groupId, Long ownerUserId, Long bookId) {
        // Endpoint cũ giữ lại để không phá flow, nhưng không còn chia sẻ toàn bộ sách mặc định.
        // Backend chỉ chia sẻ CÁC TẬP mà chủ nhóm đã mua trong sách đó.
        ensureFamilyOwner(groupId, ownerUserId);
        String model = effectiveAccessModelForBook(bookId);
        if (!"PAID".equals(model)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_PAID_BOOK_CAN_BE_SHARED");
        }
        if (!isReadableBook(bookId)) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_CONTENT_NOT_AVAILABLE");
        }

        List<Long> volumeIds = purchasedReadablePaidVolumeIdsForBook(ownerUserId, bookId);
        if (volumeIds.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "OWNER_HAS_NOT_PURCHASED_VOLUME");
        }

        for (Long volumeId : volumeIds) {
            upsertFamilySharedVolume(groupId, volumeId);
        }
        return familyDataForUser(ownerUserId);
    }

    public Map<String, Object> removeFamilySharedBook(Long groupId, Long bookId, Long ownerUserId) {
        ensureFamilyOwner(groupId, ownerUserId);
        int updated = jdbcTemplate.update(
                """
                UPDATE family_shared_volumes fsv
                SET status = 'REVOKED', revoked_at = now()
                FROM book_volumes bv
                WHERE fsv.volume_id = bv.id
                  AND fsv.group_id = ?
                  AND bv.book_id = ?
                  AND fsv.status = 'ACTIVE'
                """,
                groupId,
                bookId
        );
        if (updated == 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "SHARED_BOOK_NOT_FOUND");
        }
        return familyDataForUser(ownerUserId);
    }

    public Map<String, Object> shareFamilyVolume(Long groupId, Long ownerUserId, Long volumeId) {
        ensureFamilyOwner(groupId, ownerUserId);
        paidReadableVolumeRow(volumeId);
        if (!hasPurchasedVolume(ownerUserId, volumeId)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "OWNER_HAS_NOT_PURCHASED_VOLUME");
        }
        upsertFamilySharedVolume(groupId, volumeId);
        return familyDataForUser(ownerUserId);
    }

    private void upsertFamilySharedVolume(Long groupId, Long volumeId) {
        List<Long> existingIds = jdbcTemplate.queryForList(
                """
                SELECT id
                FROM family_shared_volumes
                WHERE group_id = ? AND volume_id = ?
                ORDER BY id DESC
                LIMIT 1
                """,
                Long.class,
                groupId,
                volumeId
        );
        if (existingIds.isEmpty()) {
            jdbcTemplate.update(
                    """
                    INSERT INTO family_shared_volumes(id, group_id, volume_id, status, shared_at, revoked_at)
                    VALUES (?, ?, ?, 'ACTIVE', now(), NULL)
                    """,
                    nextId("family_shared_volumes"),
                    groupId,
                    volumeId
            );
        } else {
            jdbcTemplate.update(
                    """
                    UPDATE family_shared_volumes
                    SET status = 'ACTIVE', shared_at = now(), revoked_at = NULL
                    WHERE id = ?
                    """,
                    existingIds.get(0)
            );
        }
    }

    public Map<String, Object> removeFamilySharedVolume(Long groupId, Long shareId, Long ownerUserId) {
        ensureFamilyOwner(groupId, ownerUserId);
        int updated = jdbcTemplate.update(
                """
                UPDATE family_shared_volumes
                SET status = 'REVOKED', revoked_at = now()
                WHERE id = ? AND group_id = ? AND status = 'ACTIVE'
                """,
                shareId,
                groupId
        );
        if (updated == 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "SHARED_VOLUME_NOT_FOUND");
        }
        return familyDataForUser(ownerUserId);
    }

    private void ensureFamilyOwner(Long groupId, Long ownerUserId) {
        Integer count = jdbcTemplate.queryForObject(
                """
                SELECT COUNT(*)
                FROM family_groups
                WHERE id = ? AND owner_user_id = ? AND status = 'ACTIVE'
                """,
                Integer.class,
                groupId,
                ownerUserId
        );
        if (count == null || count == 0) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_FAMILY_OWNER_CAN_MANAGE_MEMBERS");
        }
    }


    /**
     * Access path mặc định mà PEP dùng để gửi request vào UCON core.
     * Hàm này chỉ đọc mô hình truy cập được cấu hình trên book_volumes,
     * không kiểm tra user đã mua sách/chia sẻ gia đình/thuê gói hay chưa.
     */
    public String accessModelOverrideForVolume(Long bookVolumeId) {
        if (bookVolumeId == null) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "VOLUME_REQUIRED");
        }

        try {
            String model = jdbcTemplate.queryForObject(
                    """
                    SELECT access_model_override
                    FROM book_volumes
                    WHERE id = ?
                    """,
                    String.class,
                    bookVolumeId
            );
            if (model == null || model.isBlank()) {
                return "FREE";
            }
            return model.trim().toUpperCase(Locale.ROOT).replace("-", "_");
        } catch (EmptyResultDataAccessException ex) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "VOLUME_NOT_FOUND");
        }
    }

    private String effectiveAccessModelForVolume(Long bookVolumeId) {
        try {
            String model = jdbcTemplate.queryForObject(
                    """
                    SELECT COALESCE(bv.access_model_override)
                    FROM book_volumes bv
                    WHERE bv.id = ?
                    """,
                    String.class,
                    bookVolumeId
            );
            return model == null || model.isBlank() ? "FREE" : model.trim().toUpperCase(Locale.ROOT);
        } catch (EmptyResultDataAccessException ex) {
            return "FREE";
        }
    }

    private boolean canReadFull(Long userId, Long bookId, Long volumeId, String accessModel) {
        String model = accessModel == null ? "FREE" : accessModel.toUpperCase(Locale.ROOT);
        if ("FREE".equals(model)) return userId != null && userId > 0;
        if ("RENTAL".equals(model)) return hasActiveRental(userId);
        if ("PAID".equals(model)) return hasPurchasedVolume(userId, volumeId) || hasSharedVolumeAccess(userId, volumeId);
        return false;
    }


    /**
     * Chuẩn bị context đọc cho PEP trước khi gọi CH.
     * Hàm này KHÔNG tạo usage session và KHÔNG tự cấp quyền đọc.
     * Nó chỉ xác định metadata, accessModel, readMode và accessPath để PEP gửi qua CH/PDP.
     */
    public Map<String, Object> prepareReadAccess(Long userId, Long volumeId) {
        if (volumeId == null) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "VOLUME_REQUIRED");
        }

        VolumeContentMetadata metadata = metadataForVolume(volumeId);
        if (!metadata.contentAvailable() || metadata.totalChunks() <= 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "CONTENT_NOT_AVAILABLE");
        }

        String accessModel = effectiveAccessModelForVolume(volumeId);
        boolean fullAccess = canReadFull(userId, metadata.bookId(), volumeId, accessModel);
        boolean preview = !fullAccess;


        String accessPath = preview ? "PREVIEW" : accessModel;

        Map<String, Object> body = metadataBody(metadata);
        body.put("userId", userId);
        body.put("accessModel", accessModel);
        body.put("accessPath", accessPath);
        body.put("readMode", preview ? "PREVIEW" : "FULL");
        body.put("previewLimit", preview ? 5 : null);
        if (preview) {
            if ("RENTAL".equals(accessModel)) body.put("reason", "RENTAL_PREVIEW_ONLY");
            else if ("PAID".equals(accessModel)) body.put("reason", "PAID_PREVIEW_ONLY");
            else body.put("reason", "GUEST_PREVIEW_ONLY");
        }
        return body;
    }

    public Map<String, Object> startReadSession(Long userId, Long volumeId) {
        if (volumeId == null) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "VOLUME_REQUIRED");
        }
        VolumeContentMetadata metadata = metadataForVolume(volumeId);
        if (!metadata.contentAvailable() || metadata.totalChunks() <= 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "CONTENT_NOT_AVAILABLE");
        }

        String accessModel = effectiveAccessModelForVolume(volumeId);
        boolean fullAccess = canReadFull(userId, metadata.bookId(), volumeId, accessModel);
        boolean preview = !fullAccess;


        String accessPath = preview ? "PREVIEW" : accessModel;
        Long sessionId = jdbcTemplate.queryForObject(
                """
                INSERT INTO usage_sessions(
                    subject_user_id, resource_type, resource_id, action_id, access_path,
                    phase, status, created_at, activated_at, last_eval_at
                ) VALUES (?, 'BOOK_VOLUME', ?, 'READ', ?, 'ONGOING', 'ACTIVE', now(), now(), now())
                RETURNING id
                """,
                Long.class,
                userId,
                volumeId,
                accessPath
        );

        Map<String, Object> body = metadataBody(metadata);
        body.put("message", "PERMIT");
        body.put("sessionId", sessionId);
        body.put("userId", userId);
        body.put("accessModel", accessModel);
        body.put("readMode", preview ? "PREVIEW" : "FULL");
        body.put("previewLimit", preview ? 5 : null);
        if (preview) {
            if ("RENTAL".equals(accessModel)) body.put("reason", "RENTAL_PREVIEW_ONLY");
            else if ("PAID".equals(accessModel)) body.put("reason", "PAID_PREVIEW_ONLY");
            else body.put("reason", "GUEST_PREVIEW_ONLY");
        }
        return body;
    }

    public Map<String, Object> endReadSessionDirect(Long sessionId) {
        int updated = jdbcTemplate.update(
                """
                UPDATE usage_sessions
                SET phase = 'POST', status = 'ENDED', ended_at = COALESCE(ended_at, now()), post_handled_at = now()
                WHERE id = ? AND status <> 'ENDED'
                """,
                sessionId
        );
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("message", updated > 0 ? "SESSION_ENDED" : "SESSION_ALREADY_ENDED");
        body.put("sessionId", sessionId);
        return body;
    }

    private Map<String, Object> metadataBody(VolumeContentMetadata metadata) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("bookVolumeId", metadata.bookVolumeId());
        body.put("bookId", metadata.bookId());
        body.put("volumeNo", metadata.volumeNo());
        body.put("bookTitle", metadata.bookTitle());
        body.put("authors", metadata.authors());
        body.put("volumeTitle", metadata.volumeTitle());
        body.put("totalChunks", metadata.totalChunks());
        body.put("totalBookChunks", metadata.totalBookChunks());
        body.put("totalVolumes", metadata.totalVolumes());
        body.put("pagesPerVolume", metadata.pagesPerVolume());
        body.put("contentAvailable", metadata.contentAvailable());
        return body;
    }


    private void ensureEpubCatalogFromFolders() {
        List<DiscoveredEpub> discovered = discoveredEpubFiles();
        if (discovered.isEmpty()) return;

        Set<String> knownPaths = new HashSet<>(jdbcTemplate.queryForList(
                """
                SELECT value
                FROM system_parameters
                WHERE parameter_name LIKE 'BOOK_%_CONTENT_FILE'
                """,
                String.class
        ));
        Set<String> knownFileNames = new HashSet<>();
        for (String knownPath : knownPaths) {
            knownFileNames.add(cleanFileName(knownPath).toLowerCase(Locale.ROOT));
        }

        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));

        for (DiscoveredEpub item : discovered) {
            String path = item.catalogPath();
            String fileKey = cleanFileName(item.fileName()).toLowerCase(Locale.ROOT);
            if (knownPaths.contains(path) || knownFileNames.contains(fileKey)) continue;

            List<String> chunks = chunksForPath(path, chunkSize);
            if (!hasReadableContent(chunks)) continue;

            EpubMeta meta = metaForEpub(path);
            String accessModel = accessModelForImportedFile(item.fileName());
            long bookId = nextId("books");
            int totalVolumes = Math.max(1, (int) Math.ceil(chunks.size() / (double) pagesPerVolume));
            int price = "PAID".equals(accessModel) ? 99000 : 0;

            jdbcTemplate.update(
                    """
                    INSERT INTO books(id, title, genre, authors, default_access_model, status)
                    VALUES (?, ?, ?, ?, ?, 'ACTIVE')
                    """,
                    bookId,
                    meta.title(),
                    "ebook-import",
                    meta.authors(),
                    accessModel
            );

            jdbcTemplate.update(
                    """
                    INSERT INTO system_parameters(id, parameter_name, value, unit)
                    VALUES (?, ?, ?, ?)
                    """,
                    nextId("system_parameters"),
                    "BOOK_" + bookId + "_CONTENT_FILE",
                    path,
                    item.unit()
            );

            for (int volumeNo = 1; volumeNo <= totalVolumes; volumeNo++) {
                jdbcTemplate.update(
                        """
                        INSERT INTO book_volumes(id, book_id, title, volume_no, price, access_model_override, status)
                        VALUES (?, ?, ?, ?, ?, ?, 'ACTIVE')
                        """,
                        nextId("book_volumes"),
                        bookId,
                        meta.title() + " - Tập " + String.format(Locale.ROOT, "%02d", volumeNo),
                        volumeNo,
                        price,
                        accessModel
                );
            }

            knownPaths.add(path);
            knownFileNames.add(fileKey);
        }
    }

    private List<DiscoveredEpub> discoveredEpubFiles() {
        Map<String, DiscoveredEpub> result = new LinkedHashMap<>();
        collectEpubFilesFromDirectory("classpath", classpathEbooksDirectory(), result);
        collectEpubFilesFromDirectory("file", new File("ebooks"), result);
        return new ArrayList<>(result.values());
    }

    private File classpathEbooksDirectory() {
        try {
            Resource dir = resourceLoader.getResource("classpath:ebooks/");
            if (dir.exists()) return dir.getFile();
        } catch (IOException ignored) {
            // When packaged as a jar, classpath resources may not be represented as normal files.
        }
        return null;
    }

    private void collectEpubFilesFromDirectory(String source, File directory, Map<String, DiscoveredEpub> result) {
        if (directory == null || !directory.exists() || !directory.isDirectory()) return;
        File[] files = directory.listFiles(file -> file.isFile() && file.getName().toLowerCase(Locale.ROOT).endsWith(".epub"));
        if (files == null || files.length == 0) return;
        List<File> sorted = new ArrayList<>(List.of(files));
        sorted.sort(Comparator.comparing(File::getName));
        for (File file : sorted) {
            String fileName = file.getName();
            String path = "classpath".equals(source)
                    ? "ebooks/" + fileName
                    : "file:ebooks/" + fileName;
            result.putIfAbsent(path, new DiscoveredEpub(path, fileName, "classpath".equals(source) ? "classpath" : "external-file"));
        }
    }

    private String accessModelForImportedFile(String fileName) {
        String lower = fileName == null ? "" : fileName.toLowerCase(Locale.ROOT).trim();
        if (lower.startsWith("free-") || lower.startsWith("[free]") || lower.contains("__free__")) return "FREE";
        if (lower.startsWith("rental-") || lower.startsWith("[rental]") || lower.contains("__rental__")) return "RENTAL";
        if (lower.startsWith("paid-") || lower.startsWith("[paid]") || lower.contains("__paid__")) return "PAID";
        return "PAID";
    }

    public Map<String, Object> catalog() {
        // Catalog chỉ trả về sách thật sự đọc được.
        // Nếu EPUB không trích xuất được nội dung, hoặc tập vượt quá số chunk thực tế, backend không đưa lên thư viện.
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));

        List<Map<String, Object>> allBooks = jdbcTemplate.queryForList(
                """
                SELECT id, title, genre, authors, default_access_model, status
                FROM books
                WHERE status = 'ACTIVE'
                ORDER BY id
                """
        );

        List<Map<String, Object>> allVolumes = jdbcTemplate.queryForList(
                """
                SELECT bv.id, bv.book_id, bv.title, bv.volume_no, bv.price, bv.access_model_override, bv.status
                FROM book_volumes bv
                JOIN books b ON b.id = bv.book_id
                WHERE bv.status = 'ACTIVE'
                  AND b.status = 'ACTIVE'
                ORDER BY bv.book_id, bv.volume_no
                """
        );

        Map<Long, List<Map<String, Object>>> volumesByBook = new LinkedHashMap<>();
        for (Map<String, Object> volume : allVolumes) {
            Long bookId = numberAsLong(volume.get("book_id"));
            if (bookId == null) continue;
            volumesByBook.computeIfAbsent(bookId, ignored -> new ArrayList<>()).add(volume);
        }

        List<Map<String, Object>> catalogBooks = new ArrayList<>();
        List<Map<String, Object>> catalogVolumes = new ArrayList<>();

        for (Map<String, Object> book : allBooks) {
            Long bookId = numberAsLong(book.get("id"));
            if (bookId == null) continue;

            String path = parameterValue("BOOK_" + bookId + "_CONTENT_FILE", null);
            if (path == null || path.isBlank()) continue;
            if (!resourceExistsForPath(path)) continue;

            ContentAvailability availability = contentAvailabilityForPath(path, chunkSize, pagesPerVolume);
            if (!availability.available()) continue;

            List<Map<String, Object>> dbVolumes = volumesByBook.getOrDefault(bookId, List.of());
            if (dbVolumes.isEmpty()) continue;

            int readableVolumeCount = 0;
            for (Map<String, Object> volume : dbVolumes) {
                int volumeNo = numberAsInt(volume.get("volume_no"), 1);
                if (volumeNo < 1 || volumeNo > availability.totalVolumes()) continue;

                int chunksInThisVolume = chunksInVolume(volumeNo, availability.totalChunks(), pagesPerVolume);
                if (chunksInThisVolume <= 0) continue;

                Map<String, Object> safeVolume = new LinkedHashMap<>(volume);
                safeVolume.put("title", "Tập " + String.format(Locale.ROOT, "%02d", volumeNo));
                safeVolume.put("volume_no", volumeNo);
                safeVolume.put("contentAvailable", true);
                safeVolume.put("totalChunks", chunksInThisVolume);
                catalogVolumes.add(safeVolume);
                readableVolumeCount++;
            }

            if (readableVolumeCount == 0) continue;

            Map<String, Object> safeBook = new LinkedHashMap<>(book);
            safeBook.put("contentAvailable", true);
            safeBook.put("readableVolumeCount", readableVolumeCount);
            safeBook.put("totalBookChunks", availability.totalChunks());
            safeBook.put("totalVolumes", availability.totalVolumes());
            catalogBooks.add(safeBook);
        }

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("books", catalogBooks);
        result.put("volumes", catalogVolumes);
        return result;
    }

    private int chunksInVolume(int volumeNo, int totalBookChunks, int pagesPerVolume) {
        int startIndex = (Math.max(1, volumeNo) - 1) * pagesPerVolume;
        int remaining = Math.max(0, totalBookChunks - startIndex);
        return Math.min(pagesPerVolume, remaining);
    }

    private ContentAvailability contentAvailabilityForPath(String path, int chunkSize, int pagesPerVolume) {
        String key = path + "::" + chunkSize + "::" + pagesPerVolume;
        return contentAvailabilityCache.computeIfAbsent(key, ignored -> {
            List<String> chunks = chunksForPath(path, chunkSize);
            int totalCharacters = chunks == null ? 0 : chunks.stream().mapToInt(String::length).sum();
            boolean available = chunks != null && !chunks.isEmpty() && totalCharacters >= 500;
            int totalChunks = chunks == null ? 0 : chunks.size();
            int totalVolumes = available ? Math.max(1, (int) Math.ceil(totalChunks / (double) pagesPerVolume)) : 0;
            return new ContentAvailability(available, totalChunks, totalCharacters, totalVolumes);
        });
    }

    private boolean isReadableBook(Long bookId) {
        if (bookId == null) return false;
        String path = parameterValue("BOOK_" + bookId + "_CONTENT_FILE", null);
        if (path == null || path.isBlank() || !resourceExistsForPath(path)) return false;
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        return contentAvailabilityForPath(path, chunkSize, pagesPerVolume).available();
    }

    private List<Map<String, Object>> readablePurchasedLibraryRows(List<Map<String, Object>> rows) {
        List<Map<String, Object>> readable = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            Long bookId = numberAsLong(row.get("book_id"));
            if (isReadableBook(bookId)) readable.add(row);
        }
        return readable;
    }

    private List<Map<String, Object>> readablePaidVolumeRowsForBook(Long bookId) {
        if (!isReadableBook(bookId)) return List.of();
        String path = parameterValue("BOOK_" + bookId + "_CONTENT_FILE", null);
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        ContentAvailability availability = contentAvailabilityForPath(path, chunkSize, pagesPerVolume);

        List<Map<String, Object>> rows = jdbcTemplate.queryForList(
                """
                SELECT bv.id, bv.volume_no, COALESCE(bv.price, 0) AS price
                FROM book_volumes bv
                JOIN books b ON b.id = bv.book_id
                WHERE bv.book_id = ?
                  AND bv.status = 'ACTIVE'
                  AND b.status = 'ACTIVE'
                  AND COALESCE(bv.access_model_override, b.default_access_model) = 'PAID'
                ORDER BY bv.volume_no, bv.id
                """,
                bookId
        );

        List<Map<String, Object>> readable = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            int volumeNo = numberAsInt(row.get("volume_no"), 1);
            if (volumeNo >= 1 && volumeNo <= availability.totalVolumes() && chunksInVolume(volumeNo, availability.totalChunks(), pagesPerVolume) > 0) {
                readable.add(row);
            }
        }
        return readable;
    }

    private List<Long> readablePaidVolumeIdsForBook(Long bookId) {
        List<Long> ids = new ArrayList<>();
        for (Map<String, Object> row : readablePaidVolumeRowsForBook(bookId)) {
            Long id = numberAsLong(row.get("id"));
            if (id != null) ids.add(id);
        }
        return ids;
    }


    private boolean resourceExistsForPath(String path) {
        if (path == null || path.isBlank()) return false;
        String normalized = path.startsWith("classpath:") || path.startsWith("file:")
                ? path
                : "classpath:" + path;
        try {
            return resourceLoader.getResource(normalized).exists();
        } catch (Exception ex) {
            return false;
        }
    }

    public Map<String, Object> syncEbooksFromAdmin(Long adminUserId) {
        ensureAdminUser(adminUserId);
        ensureEpubCatalogFromFolders();
        serverTextCache.clear();
        epubMetaCache.clear();
        contentAvailabilityCache.clear();
        Map<String, Object> result = catalog();
        result.put("message", "SYNCED");
        result.put("adminUserId", adminUserId);
        return result;
    }

    private void ensureAdminUser(Long userId) {
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "ADMIN_LOGIN_REQUIRED");
        }
        long demoAdminId = 1L;
        try {
            demoAdminId = Long.parseLong(parameterValue("UCON_ADMIN_USER_ID", "1"));
        } catch (Exception ignored) {
            demoAdminId = 1L;
        }
        if (userId != demoAdminId) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ADMIN_ONLY_CAN_IMPORT_BOOKS");
        }
    }

    private Long firstVolumeIdForBook(Long bookId) {
        try {
            return jdbcTemplate.queryForObject(
                    "SELECT MIN(id) FROM book_volumes WHERE book_id = ? AND status = 'ACTIVE'",
                    Long.class,
                    bookId
            );
        } catch (EmptyResultDataAccessException ex) {
            return null;
        }
    }

    public ChunkPayload fetchChunk(Long sessionId, Long fallbackBookVolumeId, int chunkNo) {
        SessionInfo session = resolveSession(sessionId, fallbackBookVolumeId);
        Long bookVolumeId = session.bookVolumeId() != null ? session.bookVolumeId() : fallbackBookVolumeId;
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);

        String accessPath = sessionAccessPath(sessionId);
        int safeChunkNo = Math.max(1, chunkNo);
        if ("PREVIEW".equalsIgnoreCase(accessPath) && safeChunkNo > 5) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "PREVIEW_LIMIT_REACHED");
        }

        int realIndex = metadata.startChunkIndex() + safeChunkNo - 1;
        String content = "";
        if (safeChunkNo <= metadata.totalChunks() && realIndex >= 0 && realIndex < metadata.allBookChunks().size()) {
            content = metadata.allBookChunks().get(realIndex);
        }

        return new ChunkPayload(
                sessionId,
                session.userId(),
                bookVolumeId,
                safeChunkNo,
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                content
        );
    }

    public ChunkPayload fetchPreviewChunk(Long userId, Long bookVolumeId, int chunkNo) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        int safeChunkNo = Math.max(1, chunkNo);
        if (safeChunkNo > 5) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "PREVIEW_LIMIT_REACHED");
        }

        int realIndex = metadata.startChunkIndex() + safeChunkNo - 1;
        String content = "";
        if (safeChunkNo <= metadata.totalChunks() && realIndex >= 0 && realIndex < metadata.allBookChunks().size()) {
            content = metadata.allBookChunks().get(realIndex);
        }

        return new ChunkPayload(
                null,
                userId,
                bookVolumeId,
                safeChunkNo,
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                content
        );
    }

    private String sessionAccessPath(Long sessionId) {
        if (sessionId == null) return "";
        try {
            String accessPath = jdbcTemplate.queryForObject(
                    "SELECT access_path FROM usage_sessions WHERE id = ?",
                    String.class,
                    sessionId
            );
            return accessPath == null ? "" : accessPath;
        } catch (EmptyResultDataAccessException ex) {
            return "";
        }
    }

    public ReaderMetadata metadataForVolumePublic(Long bookVolumeId) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        return new ReaderMetadata(
                bookVolumeId,
                metadata.bookId(),
                metadata.volumeNo(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.contentAvailable()
        );
    }

    public List<Map<String, Object>> readHistoryForUser(Long userId, int days, int limit) {
        int safeDays = Math.max(1, Math.min(days, 60));
        int safeLimit = Math.max(1, Math.min(limit, 50));
        return jdbcTemplate.queryForList(
                """
                WITH ranked AS (
                    SELECT
                        us.id,
                        us.subject_user_id,
                        us.resource_type,
                        us.resource_id,
                        us.resource_id AS book_volume_id,
                        us.action_id,
                        us.access_path,
                        us.phase,
                        us.status,
                        to_char(us.created_at, 'YYYY-MM-DD HH24:MI:SS') AS created_at,
                        to_char(us.activated_at, 'YYYY-MM-DD HH24:MI:SS') AS activated_at,
                        to_char(us.last_eval_at, 'YYYY-MM-DD HH24:MI:SS') AS last_eval_at,
                        to_char(us.ended_at, 'YYYY-MM-DD HH24:MI:SS') AS ended_at,
                        to_char(us.revoked_at, 'YYYY-MM-DD HH24:MI:SS') AS revoked_at,
                        us.revoked_reason,
                        to_char(us.post_handled_at, 'YYYY-MM-DD HH24:MI:SS') AS post_handled_at,
                        bv.book_id AS book_id,
                        bv.volume_no AS volume_no,
                        bv.title AS volume_title,
                        b.title AS book_title,
                        b.authors AS authors,
                        ROW_NUMBER() OVER (
                            PARTITION BY b.id
                            ORDER BY COALESCE(us.ended_at, us.post_handled_at, us.last_eval_at, us.activated_at, us.created_at) DESC
                        ) AS rn,
                        COALESCE(us.ended_at, us.post_handled_at, us.last_eval_at, us.activated_at, us.created_at) AS sort_at
                    FROM usage_sessions us
                    JOIN book_volumes bv ON bv.id = us.resource_id
                    JOIN books b ON b.id = bv.book_id
                    WHERE us.subject_user_id = ?
                      AND us.resource_type = 'BOOK_VOLUME'
                      AND us.action_id = 'READ'
                      AND us.status = 'ENDED'
                      AND us.created_at >= now() - (? * interval '1 day')
                )
                SELECT
                    id,
                    subject_user_id,
                    resource_type,
                    resource_id,
                    book_volume_id,
                    action_id,
                    access_path,
                    phase,
                    status,
                    created_at,
                    activated_at,
                    last_eval_at,
                    ended_at,
                    revoked_at,
                    revoked_reason,
                    post_handled_at,
                    book_id,
                    volume_no,
                    volume_title,
                    book_title,
                    authors
                FROM ranked
                WHERE rn = 1
                ORDER BY sort_at DESC
                LIMIT ?
                """,
                userId,
                safeDays,
                safeLimit
        );
    }

    public String syncBookTitleFromEpub(Long bookVolumeId) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        if (metadata.bookId() == null || !metadata.contentAvailable()) {
            return metadata.bookTitle();
        }

        jdbcTemplate.update(
                "UPDATE books SET title = ?, authors = ? WHERE id = ?",
                metadata.bookTitle(),
                metadata.authors(),
                metadata.bookId()
        );
        jdbcTemplate.update(
                """
                UPDATE book_volumes
                SET title = 'T\u1eadp ' || LPAD(volume_no::text, 2, '0')
                WHERE book_id = ?
                """,
                metadata.bookId()
        );
        return metadata.bookTitle();
    }

    public void syncAllBookMetadataFromEpub() {
        List<Long> volumeIds = jdbcTemplate.queryForList(
                """
                SELECT MIN(id) AS id
                FROM book_volumes
                GROUP BY book_id
                ORDER BY MIN(book_id)
                """,
                Long.class
        );
        for (Long volumeId : volumeIds) {
            syncBookTitleFromEpub(volumeId);
        }
    }

    private SessionInfo resolveSession(Long sessionId, Long fallbackBookVolumeId) {
        if (sessionId == null) {
            return new SessionInfo(null, fallbackBookVolumeId);
        }
        try {
            return jdbcTemplate.queryForObject(
                    """
                    SELECT subject_user_id, resource_id
                    FROM usage_sessions
                    WHERE id = ?
                    """,
                    (rs, rowNum) -> new SessionInfo(
                            rs.getLong("subject_user_id"),
                            rs.getLong("resource_id")
                    ),
                    sessionId
            );
        } catch (EmptyResultDataAccessException ex) {
            return new SessionInfo(null, fallbackBookVolumeId);
        }
    }

    private VolumeInfo volumeInfo(Long bookVolumeId) {
        if (bookVolumeId == null) {
            return new VolumeInfo(null, null, 1, "T\u1eadp 01");
        }
        try {
            return jdbcTemplate.queryForObject(
                    """
                    SELECT bv.id, bv.book_id, bv.volume_no, bv.title
                    FROM book_volumes bv
                    WHERE bv.id = ?
                    """,
                    (rs, rowNum) -> new VolumeInfo(
                            rs.getLong("id"),
                            rs.getLong("book_id"),
                            rs.getInt("volume_no"),
                            rs.getString("title")
                    ),
                    bookVolumeId
            );
        } catch (EmptyResultDataAccessException ex) {
            return new VolumeInfo(bookVolumeId, null, Math.max(1, bookVolumeId.intValue()), "T\u1eadp " + bookVolumeId);
        }
    }

    private VolumeContentMetadata metadataForVolume(Long bookVolumeId) {
        VolumeInfo volume = volumeInfo(bookVolumeId);
        String path = contentPathForVolume(volume);
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        List<String> allChunks = chunksForPath(path, chunkSize);

        int totalBookChunks = allChunks.size();
        int totalVolumes = Math.max(1, (int) Math.ceil(totalBookChunks / (double) pagesPerVolume));
        int volumeNo = Math.max(1, volume.volumeNo());
        int startChunkIndex = (volumeNo - 1) * pagesPerVolume;
        int remaining = Math.max(0, totalBookChunks - startChunkIndex);
        int totalChunksInVolume = Math.min(pagesPerVolume, remaining);
        if (totalChunksInVolume == 0 && totalBookChunks > 0 && volumeNo == 1) {
            totalChunksInVolume = Math.min(pagesPerVolume, totalBookChunks);
        }

        EpubMeta meta = metaForEpub(path);
        String volumeTitle = "T\u1eadp " + String.format(Locale.ROOT, "%02d", volumeNo);

        return new VolumeContentMetadata(
                volume.bookId(),
                volume.id(),
                volumeNo,
                meta.title(),
                meta.authors(),
                volumeTitle,
                allChunks,
                startChunkIndex,
                totalChunksInVolume,
                totalBookChunks,
                totalVolumes,
                pagesPerVolume,
                hasReadableContent(allChunks) && totalChunksInVolume > 0
        );
    }

    private boolean hasReadableContent(List<String> chunks) {
        if (chunks == null || chunks.isEmpty()) return false;
        int totalCharacters = chunks.stream().mapToInt(String::length).sum();
        return totalCharacters >= 500;
    }

    private String contentPathForVolume(VolumeInfo volume) {
        if (volume.id() != null) {
            String volumeSpecific = parameterValue("BOOK_VOLUME_" + volume.id() + "_CONTENT_FILE", null);
            if (volumeSpecific != null && !volumeSpecific.isBlank()) return volumeSpecific;
        }
        if (volume.bookId() != null) {
            String bookSpecific = parameterValue("BOOK_" + volume.bookId() + "_CONTENT_FILE", null);
            if (bookSpecific != null && !bookSpecific.isBlank()) return bookSpecific;
        }
        return DEFAULT_EPUB_PATH;
    }

    private List<String> chunksForPath(String path, int chunkSize) {
        String cacheKey = path + "::" + chunkSize;
        return serverTextCache.computeIfAbsent(cacheKey, ignored -> splitIntoChunks(readEpubText(path), chunkSize));
    }

    private String parameterValue(String key, String defaultValue) {
        try {
            String value = jdbcTemplate.queryForObject(
                    "SELECT value FROM system_parameters WHERE parameter_name = ?",
                    String.class,
                    key
            );
            return value == null || value.isBlank() ? defaultValue : value.trim();
        } catch (EmptyResultDataAccessException ex) {
            return defaultValue;
        }
    }

    private int intParameterValue(String key, int defaultValue) {
        try {
            return Integer.parseInt(parameterValue(key, String.valueOf(defaultValue)));
        } catch (NumberFormatException ex) {
            return defaultValue;
        }
    }

    private String readEpubText(String classpathLocation) {
        if (classpathLocation == null || classpathLocation.isBlank()) return "";
        String normalized = classpathLocation.startsWith("classpath:") || classpathLocation.startsWith("file:")
                ? classpathLocation
                : "classpath:" + classpathLocation;
        Resource resource = resourceLoader.getResource(normalized);

        if (!resource.exists()) {
            return "";
        }

        // Một số EPUB có tên entry/encoding khác UTF-8. Thử UTF-8 trước, sau đó fallback ISO-8859-1
        // để tránh tình trạng sách hợp lệ nhưng backend trích xuất rỗng.
        for (Charset charset : List.of(StandardCharsets.UTF_8, StandardCharsets.ISO_8859_1)) {
            try {
                String text = readEpubTextWithCharset(resource, charset);
                if (!text.isBlank()) return text;
            } catch (Exception ignored) {
                // thử charset tiếp theo
            }
        }
        return "";
    }

    private String readEpubTextWithCharset(Resource resource, Charset charset) throws IOException {
        try (InputStream inputStream = resource.getInputStream();
             ZipInputStream zip = new ZipInputStream(inputStream, charset)) {

            EpubPackage epubPackage = readEpubPackage(resource);
            Map<String, byte[]> htmlFiles = new LinkedHashMap<>();
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null) {
                String name = entry.getName();
                String lower = name.toLowerCase(Locale.ROOT);
                if (entry.isDirectory()) continue;
                if (!(lower.endsWith(".xhtml") || lower.endsWith(".html") || lower.endsWith(".htm"))) continue;
                if (lower.contains("nav") || lower.contains("toc") || lower.contains("cover")) continue;
                htmlFiles.put(normalizePath(name), readCurrentZipEntry(zip));
            }

            List<String> orderedNames = epubPackage.spineHtmlPaths().isEmpty()
                    ? new ArrayList<>(htmlFiles.keySet())
                    : epubPackage.spineHtmlPaths();

            List<EpubSection> sections = new ArrayList<>();
            for (String sectionName : orderedNames) {
                byte[] bytes = htmlFiles.get(normalizePath(sectionName));
                if (bytes == null) continue;
                String html = decodeTextBytes(bytes);
                String text = cleanHtml(html);
                if (!text.isBlank()) {
                    sections.add(new EpubSection(sectionName, text));
                }
            }

            if (sections.isEmpty()) {
                htmlFiles.entrySet().stream()
                        .sorted(Map.Entry.comparingByKey())
                        .forEach(htmlEntry -> {
                            String text = cleanHtml(decodeTextBytes(htmlEntry.getValue()));
                            if (!text.isBlank()) sections.add(new EpubSection(htmlEntry.getKey(), text));
                        });
            }

            return String.join("\n\n", sections.stream().map(EpubSection::text).toList()).trim();
        }
    }

    private EpubMeta metaForEpub(String classpathLocation) {
        return epubMetaCache.computeIfAbsent(classpathLocation, this::extractMetaFromEpubOrFileName);
    }

    private EpubMeta extractMetaFromEpubOrFileName(String classpathLocation) {
        if (classpathLocation == null || classpathLocation.isBlank()) {
            return new EpubMeta("Ebook", "Đang cập nhật");
        }
        String normalized = classpathLocation.startsWith("classpath:") || classpathLocation.startsWith("file:")
                ? classpathLocation
                : "classpath:" + classpathLocation;
        Resource resource = resourceLoader.getResource(normalized);
        if (resource.exists()) {
            try {
                EpubPackage epubPackage = readEpubPackage(resource);
                String title = cleanInlineText(epubPackage.title());
                String authors = cleanInlineText(epubPackage.authors());
                if (!title.isBlank()) {
                    return new EpubMeta(title, authors.isBlank() ? "\u0110ang c\u1eadp nh\u1eadt" : authors);
                }
            } catch (Exception ignored) {
                // fallback to file name below
            }
        }
        return new EpubMeta(titleFromFileName(classpathLocation), authorFromFileName(classpathLocation));
    }

    private EpubPackage readEpubPackage(Resource resource) throws IOException {
        for (Charset charset : List.of(StandardCharsets.UTF_8, StandardCharsets.ISO_8859_1)) {
            try {
                EpubPackage pkg = readEpubPackageWithCharset(resource, charset);
                if (!pkg.spineHtmlPaths().isEmpty() || !cleanInlineText(pkg.title()).isBlank()) return pkg;
            } catch (Exception ignored) {
                // thử charset tiếp theo
            }
        }
        return new EpubPackage("", "", List.of());
    }

    private EpubPackage readEpubPackageWithCharset(Resource resource, Charset charset) throws IOException {
        try (InputStream inputStream = resource.getInputStream();
             ZipInputStream zip = new ZipInputStream(inputStream, charset)) {
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null) {
                String name = entry.getName().toLowerCase(Locale.ROOT);
                if (entry.isDirectory() || !name.endsWith(".opf")) continue;
                String opfPath = normalizePath(entry.getName());
                String opf = decodeTextBytes(readCurrentZipEntry(zip));
                return parseOpf(opfPath, opf);
            }
        }
        return new EpubPackage("", "", List.of());
    }

    private EpubPackage parseOpf(String opfPath, String opf) {
        String title = findDcTag(opf, "title");
        String authors = findDcTag(opf, "creator");

        Map<String, String> manifest = new HashMap<>();
        Matcher itemMatcher = Pattern.compile("(?is)<item\\s+([^>]+)>").matcher(opf);
        while (itemMatcher.find()) {
            Map<String, String> attrs = parseAttributes(itemMatcher.group(1));
            String id = attrs.get("id");
            String href = attrs.get("href");
            if (id != null && href != null) {
                manifest.put(id, href);
            }
        }

        String base = "";
        int slash = opfPath.lastIndexOf('/');
        if (slash >= 0) base = opfPath.substring(0, slash + 1);

        List<String> spine = new ArrayList<>();
        Matcher itemRefMatcher = Pattern.compile("(?is)<itemref\\s+([^>]+)>").matcher(opf);
        while (itemRefMatcher.find()) {
            Map<String, String> attrs = parseAttributes(itemRefMatcher.group(1));
            String href = manifest.get(attrs.get("idref"));
            if (href != null && !href.isBlank()) {
                spine.add(normalizePath(base + href));
            }
        }

        return new EpubPackage(title, authors, spine);
    }

    private Map<String, String> parseAttributes(String value) {
        Map<String, String> result = new HashMap<>();
        Matcher matcher = Pattern.compile("([A-Za-z_:][-A-Za-z0-9_:.]*)\\s*=\\s*[\"']([^\"']*)[\"']").matcher(value);
        while (matcher.find()) {
            result.put(matcher.group(1), decodeBasicEntities(matcher.group(2)));
        }
        return result;
    }

    private String findDcTag(String opf, String tag) {
        Matcher matcher = Pattern.compile("(?is)<dc:" + tag + "[^>]*>(.*?)</dc:" + tag + ">").matcher(opf);
        if (matcher.find()) {
            return cleanInlineText(decodeBasicEntities(matcher.group(1).replaceAll("(?is)<[^>]+>", " ")));
        }
        return "";
    }

    private String normalizePath(String value) {
        String normalized = value.replace('\\', '/');
        while (normalized.contains("//")) normalized = normalized.replace("//", "/");
        List<String> parts = new ArrayList<>();
        for (String part : normalized.split("/")) {
            if (part.isBlank() || part.equals(".")) continue;
            if (part.equals("..")) {
                if (!parts.isEmpty()) parts.remove(parts.size() - 1);
            } else {
                parts.add(part);
            }
        }
        return String.join("/", parts);
    }

    private String titleFromFileName(String value) {
        String fileName = cleanFileName(value);
        if (fileName.contains(" - ")) fileName = fileName.substring(0, fileName.indexOf(" - "));
        fileName = fileName.replaceAll("[-_]+", " ").replaceAll("\\s+", " ").trim();
        return toTitleCase(fileName.isBlank() ? "Ebook" : fileName);
    }

    private String authorFromFileName(String value) {
        String fileName = cleanFileName(value);
        if (fileName.contains(" - ")) return fileName.substring(fileName.indexOf(" - ") + 3).trim();
        return "\u0110ang c\u1eadp nh\u1eadt";
    }

    private String cleanFileName(String value) {
        String fileName = value == null ? "Ebook" : value;
        int slash = Math.max(fileName.lastIndexOf('/'), fileName.lastIndexOf('\\'));
        if (slash >= 0) fileName = fileName.substring(slash + 1);
        return fileName.replaceFirst("(?i)\\.epub$", "").trim();
    }

    private String toTitleCase(String value) {
        String[] parts = value.split(" ");
        List<String> titled = new ArrayList<>();
        for (String part : parts) {
            if (part.isBlank()) continue;
            if (part.matches("\\d+")) titled.add(part);
            else if (part.length() == 1) titled.add(part.toUpperCase(Locale.ROOT));
            else titled.add(part.substring(0, 1).toUpperCase(Locale.ROOT) + part.substring(1).toLowerCase(Locale.ROOT));
        }
        return String.join(" ", titled);
    }

    private String cleanInlineText(String value) {
        return value == null ? "" : value.replaceAll("\\s+", " ").trim();
    }

    private byte[] readCurrentZipEntry(ZipInputStream zip) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        byte[] buffer = new byte[8192];
        int read;
        while ((read = zip.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
        return out.toByteArray();
    }

    private String cleanHtml(String html) {
        String value = html
                .replaceAll("(?is)<script[^>]*>.*?</script>", " ")
                .replaceAll("(?is)<style[^>]*>.*?</style>", " ")
                .replaceAll("(?is)<head[^>]*>.*?</head>", " ")
                .replaceAll("(?i)<br\\s*/?>", "\n")
                .replaceAll("(?i)</(p|div|h[1-6]|section|article|chapter|li)>", "\n\n")
                .replaceAll("(?is)<[^>]+>", " ");

        value = decodeBasicEntities(value);
        value = Pattern.compile("[ \\t\\x0B\\f\\r]+").matcher(value).replaceAll(" ");
        value = Pattern.compile("\\n[ \\t]+").matcher(value).replaceAll("\n");
        value = Pattern.compile("\\n{3,}").matcher(value).replaceAll("\n\n");
        return value.trim();
    }

    private String decodeTextBytes(byte[] bytes) {
        if (bytes == null || bytes.length == 0) return "";

        String header = new String(bytes, 0, Math.min(bytes.length, 2048), StandardCharsets.ISO_8859_1);
        String charsetName = "UTF-8";

        Matcher xmlEncoding = Pattern.compile("(?i)encoding\\s*=\\s*['\\\"]([^'\\\"]+)['\\\"]").matcher(header);
        if (xmlEncoding.find()) {
            charsetName = xmlEncoding.group(1).trim();
        } else {
            Matcher metaCharset = Pattern.compile("(?i)charset\\s*=\\s*['\\\"]?([^\\s'\\\"/>;]+)").matcher(header);
            if (metaCharset.find()) charsetName = metaCharset.group(1).trim();
        }

        try {
            return new String(bytes, Charset.forName(charsetName));
        } catch (Exception ignored) {
            return new String(bytes, StandardCharsets.UTF_8);
        }
    }

    private String decodeBasicEntities(String value) {
        if (value == null) return "";
        String decoded = value
                .replace("&nbsp;", " ")
                .replace("&#160;", " ")
                .replace("&amp;", "&")
                .replace("&lt;", "<")
                .replace("&gt;", ">")
                .replace("&quot;", "\"")
                .replace("&apos;", "'")
                .replace("&#39;", "'")
                .replace("&ldquo;", "\u201c")
                .replace("&rdquo;", "\u201d")
                .replace("&lsquo;", "\u2018")
                .replace("&rsquo;", "\u2019")
                .replace("&mdash;", "\u2014")
                .replace("&ndash;", "\u2013")
                .replace("&hellip;", "\u2026");

        decoded = decodeNumericEntityPattern(decoded, Pattern.compile("&#(\\d+);"), 10);
        decoded = decodeNumericEntityPattern(decoded, Pattern.compile("&#x([0-9a-fA-F]+);"), 16);
        return decoded;
    }

    private String decodeNumericEntityPattern(String value, Pattern pattern, int radix) {
        Matcher matcher = pattern.matcher(value);
        StringBuffer buffer = new StringBuffer();
        while (matcher.find()) {
            String replacement;
            try {
                int codePoint = Integer.parseInt(matcher.group(1), radix);
                replacement = new String(Character.toChars(codePoint));
            } catch (Exception ignored) {
                replacement = matcher.group(0);
            }
            matcher.appendReplacement(buffer, Matcher.quoteReplacement(replacement));
        }
        matcher.appendTail(buffer);
        return buffer.toString();
    }

    private List<String> splitIntoChunks(String text, int chunkSize) {
        if (text == null || text.isBlank()) return List.of();
        int safeChunkSize = Math.max(1200, chunkSize);
        String[] paragraphs = text.split("\\n{2,}");
        List<String> chunks = new ArrayList<>();
        StringBuilder current = new StringBuilder();

        for (String rawParagraph : paragraphs) {
            String paragraph = rawParagraph.trim();
            if (paragraph.isBlank()) continue;

            if (current.length() > 0 && current.length() + paragraph.length() + 2 > safeChunkSize) {
                chunks.add(current.toString().trim());
                current.setLength(0);
            }

            if (paragraph.length() > safeChunkSize) {
                if (current.length() > 0) {
                    chunks.add(current.toString().trim());
                    current.setLength(0);
                }
                for (int start = 0; start < paragraph.length(); start += safeChunkSize) {
                    int end = Math.min(paragraph.length(), start + safeChunkSize);
                    String part = paragraph.substring(start, end).trim();
                    if (!part.isBlank()) chunks.add(part);
                }
            } else {
                if (current.length() > 0) current.append("\n\n");
                current.append(paragraph);
            }
        }

        if (current.length() > 0) {
            chunks.add(current.toString().trim());
        }

        return chunks;
    }

    private record SessionInfo(Long userId, Long bookVolumeId) {}
    private record VolumeInfo(Long id, Long bookId, int volumeNo, String dbTitle) {}
    private record ContentAvailability(boolean available, int totalChunks, int totalCharacters, int totalVolumes) {}

    private record EpubSection(String name, String text) {}
    private record DiscoveredEpub(String catalogPath, String fileName, String unit) {}

    private record EpubMeta(String title, String authors) {}
    private record EpubPackage(String title, String authors, List<String> spineHtmlPaths) {}

    private record VolumeContentMetadata(
            Long bookId,
            Long bookVolumeId,
            int volumeNo,
            String bookTitle,
            String authors,
            String volumeTitle,
            List<String> allBookChunks,
            int startChunkIndex,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            boolean contentAvailable
    ) {}

    public record ReaderMetadata(
            Long bookVolumeId,
            Long bookId,
            int volumeNo,
            String bookTitle,
            String authors,
            String volumeTitle,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            boolean contentAvailable
    ) {}

    public record ChunkPayload(
            Long sessionId,
            Long userId,
            Long bookVolumeId,
            int chunkNo,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            String bookTitle,
            String authors,
            String volumeTitle,
            String content
    ) {}
}
