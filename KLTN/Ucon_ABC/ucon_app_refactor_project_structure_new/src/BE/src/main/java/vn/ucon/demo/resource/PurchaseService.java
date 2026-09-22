package vn.ucon.demo.resource;

import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

@Service
public class PurchaseService {

    private final JdbcTemplate jdbcTemplate;
    private final EbookReadService readService;

    public PurchaseService(JdbcTemplate jdbcTemplate, EbookReadService readService) {
        this.jdbcTemplate = jdbcTemplate;
        this.readService = readService;
    }

    private Long numberAsLong(Object value) {
        return value instanceof Number number ? number.longValue() : null;
    }

    private long nextId(String tableName) {
        String safeTable = tableName.replaceAll("[^a-zA-Z0-9_]", "");
        Long value = jdbcTemplate.queryForObject("SELECT COALESCE(MAX(id), 0) + 1 FROM " + safeTable, Long.class);
        return value == null ? 1L : value;
    }

    public boolean hasPurchasedBook(Long userId, Long bookId) {
        // Trong bản này, "mua sách" được hiểu là user đã mua ÍT NHẤT MỘT tập của sách.
        // Quyền đọc đầy đủ vẫn kiểm tra theo từng tập bằng hasPurchasedVolume(...).
        return !purchasedReadablePaidVolumeIdsForBook(userId, bookId).isEmpty();
    }

    public List<Long> purchasedReadablePaidVolumeIdsForBook(Long userId, Long bookId) {
        if (userId == null || userId <= 0 || bookId == null) return List.of();
        List<Long> readablePaidVolumes = readService.readablePaidVolumeIdsForBook(bookId);
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
        return readService.readablePurchasedLibraryRows(rows);
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
        String bookAccessModel = readService.effectiveAccessModelForBook(bookId);
        if (!"PAID".equals(bookAccessModel)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_PAID_BOOK_CAN_BE_PURCHASED");
        }

        List<Map<String, Object>> volumes = readService.readablePaidVolumeRowsForBook(bookId);
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

    public Map<String, Object> paidReadableVolumeRow(Long volumeId) {
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

        EbookReadService.ReaderMetadata metadata = readService.metadataForVolumePublic(volumeId);
        if (!metadata.contentAvailable() || metadata.totalChunks() <= 0) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_CONTENT_NOT_AVAILABLE");
        }
        return volume;
    }
}
