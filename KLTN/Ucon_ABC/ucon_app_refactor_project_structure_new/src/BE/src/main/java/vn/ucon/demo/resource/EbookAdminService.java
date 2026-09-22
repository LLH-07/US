package vn.ucon.demo.resource;

import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.util.Map;

@Service
public class EbookAdminService {

    private final JdbcTemplate jdbcTemplate;
    private final EbookReadService readService;

    public EbookAdminService(JdbcTemplate jdbcTemplate, EbookReadService readService) {
        this.jdbcTemplate = jdbcTemplate;
        this.readService = readService;
    }

    public Map<String, Object> syncEbooksFromAdmin(Long adminUserId) {
        ensureAdminUser(adminUserId);
        readService.syncCatalogFromFolders();

        Map<String, Object> result = new java.util.LinkedHashMap<>(readService.catalog());
        result.put("message", "SYNCED");
        result.put("adminUserId", adminUserId);
        return result;
    }

    private void ensureAdminUser(Long userId) {
        if (userId == null || userId <= 0) {
            throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "ADMIN_LOGIN_REQUIRED");
        }

        Long configuredAdminId = jdbcTemplate.query(
                """
                SELECT parameter_value
                FROM system_parameters
                WHERE parameter_name = 'UCON_ADMIN_USER_ID'
                """,
                rs -> rs.next() ? Long.parseLong(rs.getString(1)) : 1L
        );

        long adminId = configuredAdminId == null ? 1L : configuredAdminId;
        if (userId != adminId) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "ADMIN_ONLY_CAN_IMPORT_BOOKS");
        }
    }
}
