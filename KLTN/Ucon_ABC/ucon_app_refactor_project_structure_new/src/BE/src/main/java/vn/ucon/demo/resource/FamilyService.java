package vn.ucon.demo.resource;

import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

@Service
public class FamilyService {

    private final JdbcTemplate jdbcTemplate;
    private final EbookReadService readService;
    private final PurchaseService purchaseService;

    public FamilyService(JdbcTemplate jdbcTemplate, EbookReadService readService, PurchaseService purchaseService) {
        this.jdbcTemplate = jdbcTemplate;
        this.readService = readService;
        this.purchaseService = purchaseService;
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

    private Long numberAsLong(Object value) {
        return value instanceof Number number ? number.longValue() : null;
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

    private long nextId(String tableName) {
        String safeTable = tableName.replaceAll("[^a-zA-Z0-9_]", "");
        Long value = jdbcTemplate.queryForObject("SELECT COALESCE(MAX(id), 0) + 1 FROM " + safeTable, Long.class);
        return value == null ? 1L : value;
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

    public Map<String, Object> shareFamilyBook(Long groupId, Long ownerUserId, Long bookId) {
        // Endpoint cũ giữ lại để không phá flow, nhưng không còn chia sẻ toàn bộ sách mặc định.
        // Backend chỉ chia sẻ CÁC TẬP mà chủ nhóm đã mua trong sách đó.
        ensureFamilyOwner(groupId, ownerUserId);
        String model = readService.effectiveAccessModelForBook(bookId);
        if (!"PAID".equals(model)) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_PAID_BOOK_CAN_BE_SHARED");
        }
        if (!readService.isReadableBook(bookId)) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_CONTENT_NOT_AVAILABLE");
        }

        List<Long> volumeIds = purchaseService.purchasedReadablePaidVolumeIdsForBook(ownerUserId, bookId);
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
        purchaseService.paidReadableVolumeRow(volumeId);
        if (!purchaseService.hasPurchasedVolume(ownerUserId, volumeId)) {
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
}
