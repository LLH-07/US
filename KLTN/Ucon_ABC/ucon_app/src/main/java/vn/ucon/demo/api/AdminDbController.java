package vn.ucon.demo.api;

import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.transaction.annotation.Isolation;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.server.ResponseStatusException;

import java.math.BigDecimal;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.time.temporal.TemporalAccessor;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;

@CrossOrigin(origins = "*")
@RestController
@RequestMapping("/api/admin/db")
public class AdminDbController {

    private record TableDef(String publicName, String physicalName, String primaryKey, boolean numericKey) {}

    private record ColumnDef(
            String name,
            String dataType,
            String udtName,
            boolean nullable,
            int ordinalPosition
    ) {}

    private final JdbcTemplate jdbcTemplate;

    public AdminDbController(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @ExceptionHandler(ResponseStatusException.class)
    public ResponseEntity<?> handleResponseStatusException(ResponseStatusException ex) {
        String reason = ex.getReason() == null ? "ADMIN_DB_REQUEST_FAILED" : ex.getReason();
        return ResponseEntity.status(ex.getStatusCode()).body(Map.of(
                "message", "ERROR",
                "reason", reason
        ));
    }

    @ExceptionHandler(DataIntegrityViolationException.class)
    public ResponseEntity<?> handleIntegrityError(DataIntegrityViolationException ex) {
        return ResponseEntity.status(HttpStatus.CONFLICT).body(Map.of(
                "message", "DELETE_BLOCKED_BY_RELATION",
                "reason", "Dòng này đang được bảng khác tham chiếu. Hãy xóa dữ liệu liên quan trước hoặc dùng cascade=true.",
                "detail", safeDetail(ex)
        ));
    }

    @Transactional(readOnly = true, isolation = Isolation.REPEATABLE_READ)
    @GetMapping("/snapshot")
    public ResponseEntity<?> snapshot() {
        Map<String, Object> body = new LinkedHashMap<>();
        Map<String, Object> tables = new LinkedHashMap<>();
        Map<String, Object> schemas = new LinkedHashMap<>();
        Map<String, Object> primaryKeys = new LinkedHashMap<>();
        Map<String, Object> metadata = new LinkedHashMap<>();

        List<TableDef> defs = tableDefs();
        for (TableDef def : defs) {
            tables.put(def.publicName(), rowsFor(def));
            schemas.put(def.publicName(), columnNames(def));
            primaryKeys.put(def.publicName(), def.primaryKey());
            metadata.put(def.publicName(), metadataFor(def));
        }

        body.put("message", "OK");
        body.put("source", "POSTGRESQL");
        body.put("snapshotIsolation", "REPEATABLE_READ");
        body.put("tables", tables);
        body.put("schemas", schemas);
        body.put("primaryKeys", primaryKeys);
        body.put("metadata", metadata);
        return ResponseEntity.ok(body);
    }

    @Transactional(readOnly = true)
    @GetMapping("/tables/{tableName}")
    public ResponseEntity<?> table(@PathVariable String tableName) {
        TableDef def = resolve(tableName);
        return ResponseEntity.ok(Map.of(
                "message", "OK",
                "source", "POSTGRESQL",
                "table", def.publicName(),
                "primaryKey", def.primaryKey(),
                "rows", rowsFor(def)
        ));
    }

    @Transactional
    @PostMapping("/tables/{tableName}/rows")
    public ResponseEntity<?> createRow(
            @PathVariable String tableName,
            @RequestBody Map<String, Object> payload
    ) {
        TableDef def = resolve(tableName);
        Map<String, Object> clean = cleanPayload(def, payload, false);
        if (clean.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "EMPTY_ROW_PAYLOAD");
        }

        StringBuilder columns = new StringBuilder();
        StringBuilder placeholders = new StringBuilder();
        List<Object> args = new ArrayList<>();

        int index = 0;
        Map<String, String> types = columnTypes(def);
        for (Map.Entry<String, Object> entry : clean.entrySet()) {
            if (index > 0) {
                columns.append(", ");
                placeholders.append(", ");
            }
            columns.append(quoted(entry.getKey()));
            placeholders.append(placeholderFor(types.get(entry.getKey())));
            args.add(normalizeInputValue(entry.getValue()));
            index++;
        }

        String sql = "INSERT INTO " + quoted(def.physicalName()) + " (" + columns + ") VALUES (" + placeholders + ") RETURNING *";
        Map<String, Object> inserted = jdbcTemplate.queryForMap(sql, args.toArray());
        return ResponseEntity.ok(Map.of(
                "message", "INSERTED_TO_POSTGRESQL",
                "table", def.publicName(),
                "row", normalizeRow(inserted)
        ));
    }

    @Transactional
    @PutMapping("/tables/{tableName}/rows/{key}")
    public ResponseEntity<?> updateRow(
            @PathVariable String tableName,
            @PathVariable String key,
            @RequestBody Map<String, Object> payload
    ) {
        TableDef def = resolve(tableName);
        Object typedKey = typedKey(def, key);
        Map<String, Object> clean = cleanPayload(def, payload, true);
        clean.remove(def.primaryKey());

        if (clean.isEmpty()) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "EMPTY_ROW_PAYLOAD");
        }

        StringBuilder sets = new StringBuilder();
        List<Object> args = new ArrayList<>();
        Map<String, String> types = columnTypes(def);

        int index = 0;
        for (Map.Entry<String, Object> entry : clean.entrySet()) {
            if (index > 0) {
                sets.append(", ");
            }
            sets.append(quoted(entry.getKey())).append(" = ").append(placeholderFor(types.get(entry.getKey())));
            args.add(normalizeInputValue(entry.getValue()));
            index++;
        }
        args.add(typedKey);

        String sql = "UPDATE " + quoted(def.physicalName()) + " SET " + sets + " WHERE " + quoted(def.primaryKey()) + " = ? RETURNING *";
        Map<String, Object> updated = jdbcTemplate.queryForMap(sql, args.toArray());
        return ResponseEntity.ok(Map.of(
                "message", "UPDATED_IN_POSTGRESQL",
                "table", def.publicName(),
                "primaryKey", def.primaryKey(),
                "key", key,
                "row", normalizeRow(updated)
        ));
    }

    @PostMapping("/query")
    public ResponseEntity<?> query(@RequestBody Map<String, Object> payload) {
        String sql = String.valueOf(payload.getOrDefault("sql", "")).trim();
        if (sql.isBlank()) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "EMPTY_SQL");
        }
        String lower = sql.toLowerCase();
        if (!lower.startsWith("select")) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ONLY_SELECT_SQL_IS_ALLOWED");
        }
        if (lower.contains(";") && !lower.endsWith(";")) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "ONE_SELECT_STATEMENT_ONLY");
        }
        String executableSql = sql.endsWith(";") ? sql.substring(0, sql.length() - 1) : sql;
        List<Map<String, Object>> rawRows = jdbcTemplate.queryForList(executableSql);
        List<Map<String, Object>> rows = new ArrayList<>();
        for (Map<String, Object> raw : rawRows) {
            rows.add(normalizeRow(raw));
            if (rows.size() >= 300) {
                break;
            }
        }
        return ResponseEntity.ok(Map.of(
                "message", "SQL_QUERY_OK",
                "source", "POSTGRESQL",
                "rowCount", rows.size(),
                "rows", rows
        ));
    }

    @Transactional
    @DeleteMapping("/tables/{tableName}/rows/{key}")
    public ResponseEntity<?> deleteRow(
            @PathVariable String tableName,
            @PathVariable String key,
            @RequestParam(defaultValue = "true") boolean cascade
    ) {
        TableDef def = resolve(tableName);
        Object typedKey = typedKey(def, key);
        int deleted = cascade ? deleteWithDependencies(def, typedKey) : deleteDirect(def, typedKey);
        return ResponseEntity.ok(Map.of(
                "message", "DELETED_FROM_POSTGRESQL",
                "table", def.publicName(),
                "primaryKey", def.primaryKey(),
                "key", key,
                "cascade", cascade,
                "deletedRows", deleted
        ));
    }

    private TableDef resolve(String tableName) {
        String input = tableName == null ? "" : tableName.trim();
        String normalized = input.toUpperCase();
        for (TableDef def : tableDefs()) {
            if (def.publicName().equals(normalized) || def.physicalName().equalsIgnoreCase(input)) {
                return def;
            }
        }
        throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "TABLE_NOT_ALLOWED");
    }

    private List<TableDef> tableDefs() {
        List<Map<String, Object>> rows = jdbcTemplate.queryForList(
                """
                SELECT
                    t.table_name,
                    COALESCE(pk.column_name, first_col.column_name) AS primary_key,
                    COALESCE(pk.udt_name, first_col.udt_name) AS primary_key_type
                FROM information_schema.tables t
                LEFT JOIN LATERAL (
                    SELECT kcu.column_name, c.udt_name
                    FROM information_schema.table_constraints tc
                    JOIN information_schema.key_column_usage kcu
                      ON tc.constraint_name = kcu.constraint_name
                     AND tc.table_schema = kcu.table_schema
                     AND tc.table_name = kcu.table_name
                    JOIN information_schema.columns c
                      ON c.table_schema = kcu.table_schema
                     AND c.table_name = kcu.table_name
                     AND c.column_name = kcu.column_name
                    WHERE tc.table_schema = 'public'
                      AND tc.constraint_type = 'PRIMARY KEY'
                      AND tc.table_name = t.table_name
                    ORDER BY kcu.ordinal_position
                    LIMIT 1
                ) pk ON true
                LEFT JOIN LATERAL (
                    SELECT column_name, udt_name
                    FROM information_schema.columns c
                    WHERE c.table_schema = 'public'
                      AND c.table_name = t.table_name
                    ORDER BY ordinal_position
                    LIMIT 1
                ) first_col ON true
                WHERE t.table_schema = 'public'
                  AND t.table_type = 'BASE TABLE'
                ORDER BY t.table_name
                """
        );

        List<TableDef> defs = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            String physicalName = String.valueOf(row.get("table_name"));
            String primaryKey = String.valueOf(row.get("primary_key"));
            String keyType = String.valueOf(row.get("primary_key_type"));
            defs.add(new TableDef(
                    toPublicName(physicalName),
                    physicalName,
                    primaryKey,
                    isNumericType(keyType)
            ));
        }
        return defs;
    }

    private String toPublicName(String physicalName) {
        return physicalName == null ? "" : physicalName.trim().toUpperCase();
    }

    private boolean isNumericType(String type) {
        String normalized = type == null ? "" : type.toLowerCase();
        return Set.of("int2", "int4", "int8", "numeric", "float4", "float8").contains(normalized);
    }

    private Object typedKey(TableDef def, String rawKey) {
        if (!def.numericKey()) {
            return rawKey;
        }
        try {
            return Long.parseLong(rawKey);
        } catch (NumberFormatException ex) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "INVALID_NUMERIC_PRIMARY_KEY");
        }
    }

    private List<Map<String, Object>> rowsFor(TableDef def) {
        String sql = "SELECT * FROM " + quoted(def.physicalName()) + " ORDER BY " + quoted(def.primaryKey());
        List<Map<String, Object>> rawRows = jdbcTemplate.queryForList(sql);
        List<Map<String, Object>> normalized = new ArrayList<>();
        for (Map<String, Object> raw : rawRows) {
            Map<String, Object> row = new LinkedHashMap<>();
            for (Map.Entry<String, Object> entry : raw.entrySet()) {
                row.put(entry.getKey(), normalize(entry.getValue()));
            }
            normalized.add(row);
        }
        return normalized;
    }

    private List<ColumnDef> columnsFor(TableDef def) {
        List<Map<String, Object>> rows = jdbcTemplate.queryForList(
                """
                SELECT column_name, data_type, udt_name, is_nullable, ordinal_position
                FROM information_schema.columns
                WHERE table_schema = 'public'
                  AND table_name = ?
                ORDER BY ordinal_position
                """,
                def.physicalName()
        );
        List<ColumnDef> columns = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            columns.add(new ColumnDef(
                    String.valueOf(row.get("column_name")),
                    String.valueOf(row.get("data_type")),
                    String.valueOf(row.get("udt_name")),
                    "YES".equalsIgnoreCase(String.valueOf(row.get("is_nullable"))),
                    ((Number) row.get("ordinal_position")).intValue()
            ));
        }
        return columns;
    }

    private List<String> columnNames(TableDef def) {
        List<String> names = new ArrayList<>();
        for (ColumnDef column : columnsFor(def)) {
            names.add(column.name());
        }
        return names;
    }

    private Map<String, Object> metadataFor(TableDef def) {
        Map<String, Object> meta = new LinkedHashMap<>();
        List<Map<String, Object>> columns = new ArrayList<>();
        for (ColumnDef column : columnsFor(def)) {
            Map<String, Object> item = new LinkedHashMap<>();
            item.put("name", column.name());
            item.put("dataType", column.dataType());
            item.put("udtName", column.udtName());
            item.put("nullable", column.nullable());
            item.put("ordinalPosition", column.ordinalPosition());
            item.put("primaryKey", column.name().equals(def.primaryKey()));
            columns.add(item);
        }
        meta.put("physicalName", def.physicalName());
        meta.put("publicName", def.publicName());
        meta.put("primaryKey", def.primaryKey());
        meta.put("numericKey", def.numericKey());
        meta.put("columns", columns);
        return meta;
    }

    private String quoted(String identifier) {
        if (identifier == null || !identifier.matches("[A-Za-z_][A-Za-z0-9_]*")) {
            throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "INVALID_DB_IDENTIFIER");
        }
        return "\"" + identifier.replace("\"", "\"\"") + "\"";
    }

    private Object normalize(Object value) {
        if (value == null) return null;
        if (value instanceof String || value instanceof Number || value instanceof Boolean || value instanceof BigDecimal) return value;
        if (value instanceof Timestamp || value instanceof TemporalAccessor) return value.toString();
        String className = value.getClass().getName().toLowerCase();
        if (className.contains("pgobject")) return value.toString();
        return String.valueOf(value);
    }

    private Map<String, String> columnTypes(TableDef def) {
        Map<String, String> types = new LinkedHashMap<>();
        for (ColumnDef column : columnsFor(def)) {
            types.put(column.name(), column.udtName());
        }
        return types;
    }

    private Map<String, Object> cleanPayload(TableDef def, Map<String, Object> payload, boolean includePrimaryKey) {
        Map<String, String> types = columnTypes(def);
        Set<String> allowed = new HashSet<>(types.keySet());
        Map<String, Object> clean = new LinkedHashMap<>();

        if (payload == null) {
            return clean;
        }

        for (Map.Entry<String, Object> entry : payload.entrySet()) {
            String column = String.valueOf(entry.getKey());
            if (column.startsWith("__") || !allowed.contains(column)) {
                continue;
            }
            Object value = entry.getValue();
            if (!includePrimaryKey && column.equals(def.primaryKey()) && isBlankValue(value)) {
                continue;
            }
            clean.put(column, convertInputValue(types.get(column), value));
        }

        return clean;
    }

    private boolean isBlankValue(Object value) {
        return value == null || String.valueOf(value).trim().isEmpty();
    }

    private Object normalizeInputValue(Object value) {
        return value;
    }

    private Object convertInputValue(String type, Object value) {
        if (value == null) {
            return null;
        }

        if (!(value instanceof String)) {
            return value;
        }

        String text = String.valueOf(value).trim();
        if (text.isEmpty() || "null".equalsIgnoreCase(text)) {
            return null;
        }

        String normalizedType = type == null ? "" : type.toLowerCase();
        try {
            return switch (normalizedType) {
                case "int2" -> Short.parseShort(text);
                case "int4" -> Integer.parseInt(text);
                case "int8" -> Long.parseLong(text);
                case "numeric" -> new BigDecimal(text);
                case "float4" -> Float.parseFloat(text);
                case "float8" -> Double.parseDouble(text);
                case "bool" -> Boolean.parseBoolean(text);
                case "timestamp", "timestamptz" -> Timestamp.valueOf(normalizeTimestampText(text));
                case "date" -> Date.valueOf(LocalDate.parse(text.substring(0, 10)));
                case "time", "timetz" -> Time.valueOf(normalizeTimeText(text));
                default -> value;
            };
        } catch (Exception ex) {
            throw new ResponseStatusException(
                    HttpStatus.BAD_REQUEST,
                    "INVALID_VALUE_FOR_COLUMN_TYPE: type=" + normalizedType + ", value=" + text
            );
        }
    }

    private LocalDateTime normalizeTimestampText(String value) {
        String normalized = value.trim().replace('T', ' ');

        if (normalized.endsWith("Z")) {
            normalized = normalized.substring(0, normalized.length() - 1);
        }

        // PostgreSQL timestamp without time zone không nhận phần offset trong Timestamp.valueOf.
        // Ví dụ: 2026-07-09 10:21:23.847015+07:00 -> 2026-07-09 10:21:23.847015
        int plusIndex = normalized.indexOf('+', 10);
        if (plusIndex > 0) {
            normalized = normalized.substring(0, plusIndex);
        }

        int minusIndex = normalized.indexOf('-', 10);
        if (minusIndex > 0) {
            normalized = normalized.substring(0, minusIndex);
        }

        if (normalized.length() == 10) {
            normalized = normalized + " 00:00:00";
        } else if (normalized.length() == 16) {
            normalized = normalized + ":00";
        }

        return LocalDateTime.parse(normalized.replace(' ', 'T'));
    }

    private LocalTime normalizeTimeText(String value) {
        String normalized = value.trim();

        int plusIndex = normalized.indexOf('+');
        if (plusIndex > 0) {
            normalized = normalized.substring(0, plusIndex);
        }

        int minusIndex = normalized.indexOf('-', 1);
        if (minusIndex > 0) {
            normalized = normalized.substring(0, minusIndex);
        }

        if (normalized.length() == 5) {
            normalized = normalized + ":00";
        }

        return LocalTime.parse(normalized);
    }

    private String placeholderFor(String type) {
        if ("jsonb".equalsIgnoreCase(type) || "json".equalsIgnoreCase(type)) {
            return "CAST(? AS jsonb)";
        }
        return "?";
    }

    private Map<String, Object> normalizeRow(Map<String, Object> raw) {
        Map<String, Object> row = new LinkedHashMap<>();
        for (Map.Entry<String, Object> entry : raw.entrySet()) {
            row.put(entry.getKey(), normalize(entry.getValue()));
        }
        return row;
    }

    private int deleteDirect(TableDef def, Object key) {
        return jdbcTemplate.update("DELETE FROM " + quoted(def.physicalName()) + " WHERE " + quoted(def.primaryKey()) + " = ?", key);
    }

    private int deleteWithDependencies(TableDef def, Object key) {
        return switch (def.publicName()) {
            case "USERS" -> deleteUser((Long) key);
            case "BOOKS" -> deleteBook((Long) key);
            case "BOOK_VOLUMES" -> deleteBookVolume((Long) key);
            case "RENTAL_PLANS" -> deleteRentalPlan((Long) key);
            case "VOLUME_PURCHASES" -> deleteVolumePurchase((Long) key);
            case "FAMILY_GROUPS" -> deleteFamilyGroup((Long) key);
            case "POLICY_REGISTRY" -> deletePolicy((Long) key);
            case "PIP_ATTRIBUTE_MAPPINGS" -> deletePipAttribute(String.valueOf(key));
            case "USAGE_SESSIONS" -> deleteUsageSession((Long) key);
            default -> deleteDirect(def, key);
        };
    }

    private int deleteUser(Long userId) {
        int deleted = 0;
        deleted += deleteUsageSessionsByUser(userId);
        for (Long groupId : jdbcTemplate.queryForList("SELECT id FROM family_groups WHERE owner_user_id = ?", Long.class, userId)) {
            deleted += deleteFamilyGroup(groupId);
        }
        deleted += jdbcTemplate.update("DELETE FROM family_group_members WHERE user_id = ?", userId);
        deleted += jdbcTemplate.update("DELETE FROM user_rentals WHERE user_id = ?", userId);
        deleted += jdbcTemplate.update("DELETE FROM volume_purchases WHERE user_id = ?", userId);
        deleted += jdbcTemplate.update("DELETE FROM users WHERE id = ?", userId);
        return deleted;
    }

    private int deleteBook(Long bookId) {
        int deleted = 0;
        for (Long volumeId : jdbcTemplate.queryForList("SELECT id FROM book_volumes WHERE book_id = ?", Long.class, bookId)) {
            deleted += deleteBookVolume(volumeId);
        }
        deleted += jdbcTemplate.update("DELETE FROM books WHERE id = ?", bookId);
        return deleted;
    }

    private int deleteBookVolume(Long volumeId) {
        int deleted = 0;
        deleted += deleteUsageSessionsByResource("BOOK_VOLUME", volumeId);
        deleted += jdbcTemplate.update("DELETE FROM volume_purchases WHERE volume_id = ?", volumeId);
        deleted += jdbcTemplate.update("DELETE FROM family_shared_volumes WHERE volume_id = ?", volumeId);
        deleted += jdbcTemplate.update("DELETE FROM book_volumes WHERE id = ?", volumeId);
        return deleted;
    }

    private int deleteRentalPlan(Long planId) {
        int deleted = 0;
        deleted += jdbcTemplate.update("DELETE FROM user_rentals WHERE plan_id = ?", planId);
        deleted += jdbcTemplate.update("DELETE FROM rental_plans WHERE id = ?", planId);
        return deleted;
    }

    private int deleteVolumePurchase(Long purchaseId) {
        Map<String, Object> purchase;
        try {
            purchase = jdbcTemplate.queryForMap(
                    "SELECT user_id, volume_id FROM volume_purchases WHERE id = ?",
                    purchaseId
            );
        } catch (Exception ex) {
            return jdbcTemplate.update("DELETE FROM volume_purchases WHERE id = ?", purchaseId);
        }

        Long userId = ((Number) purchase.get("user_id")).longValue();
        Long volumeId = ((Number) purchase.get("volume_id")).longValue();
        int deleted = 0;

        // Nếu chủ nhóm bị xóa quyền mua tập này, mọi share của chính tập đó từ các nhóm do user làm chủ cũng phải mất hiệu lực.
        deleted += jdbcTemplate.update(
                """
                DELETE FROM family_shared_volumes fsv
                USING family_groups g
                WHERE g.id = fsv.group_id
                  AND g.owner_user_id = ?
                  AND fsv.volume_id = ?
                """,
                userId,
                volumeId
        );
        deleted += jdbcTemplate.update("DELETE FROM volume_purchases WHERE id = ?", purchaseId);
        return deleted;
    }

    private int deleteFamilyGroup(Long groupId) {
        int deleted = 0;
        deleted += jdbcTemplate.update("DELETE FROM family_shared_volumes WHERE group_id = ?", groupId);
        deleted += jdbcTemplate.update("DELETE FROM family_group_members WHERE group_id = ?", groupId);
        deleted += jdbcTemplate.update("DELETE FROM family_groups WHERE id = ?", groupId);
        return deleted;
    }

    private int deletePolicy(Long policyId) {
        int deleted = 0;
        deleted += jdbcTemplate.update("DELETE FROM obligation_executions WHERE policy_id = ?", policyId);
        deleted += jdbcTemplate.update("DELETE FROM usage_session_policies WHERE policy_id = ?", policyId);
        deleted += jdbcTemplate.update("DELETE FROM policy_attribute_requirements WHERE policy_id = ?", policyId);
        deleted += jdbcTemplate.update("DELETE FROM policy_registry WHERE id = ?", policyId);
        return deleted;
    }

    private int deletePipAttribute(String attrKey) {
        int deleted = 0;
        deleted += jdbcTemplate.update("DELETE FROM policy_attribute_requirements WHERE attr_key = ?", attrKey);
        deleted += jdbcTemplate.update("DELETE FROM pip_update_actions WHERE attr_key = ?", attrKey);
        deleted += jdbcTemplate.update("DELETE FROM pip_attribute_mappings WHERE attr_key = ?", attrKey);
        return deleted;
    }

    private int deleteUsageSessionsByUser(Long userId) {
        int deleted = 0;
        for (Long sessionId : jdbcTemplate.queryForList("SELECT id FROM usage_sessions WHERE subject_user_id = ?", Long.class, userId)) {
            deleted += deleteUsageSession(sessionId);
        }
        return deleted;
    }

    private int deleteUsageSessionsByResource(String resourceType, Long resourceId) {
        int deleted = 0;
        for (Long sessionId : jdbcTemplate.queryForList(
                "SELECT id FROM usage_sessions WHERE resource_type = ? AND resource_id = ?",
                Long.class,
                resourceType,
                resourceId
        )) {
            deleted += deleteUsageSession(sessionId);
        }
        return deleted;
    }

    private int deleteUsageSession(Long sessionId) {
        int deleted = 0;
        deleted += jdbcTemplate.update("DELETE FROM obligation_executions WHERE usage_session_id = ?", sessionId);
        deleted += jdbcTemplate.update("DELETE FROM session_events WHERE usage_session_id = ?", sessionId);
        deleted += jdbcTemplate.update("DELETE FROM usage_session_dependencies WHERE usage_session_id = ?", sessionId);
        deleted += jdbcTemplate.update("DELETE FROM usage_session_policies WHERE usage_session_id = ?", sessionId);
        deleted += jdbcTemplate.update("DELETE FROM usage_sessions WHERE id = ?", sessionId);
        return deleted;
    }

    private String safeDetail(DataIntegrityViolationException ex) {
        String message = ex.getMostSpecificCause() == null ? ex.getMessage() : ex.getMostSpecificCause().getMessage();
        if (message == null) return "";
        return message.length() > 600 ? message.substring(0, 600) + "..." : message;
    }
}