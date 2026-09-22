package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

import java.sql.Timestamp;

@Component
public class RequestAttributeReader implements AttributeReader {

    private final JdbcTemplate jdbcTemplate;

    public RequestAttributeReader(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return "RequestAttributeReader".equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        return switch (context.attrKey()) {
            case "ucon.phase" -> context.request().phase();
            case "action.id" -> context.request().actionId();
            case "access.path" -> context.request().accessPath();
            case "resource.type" -> context.request().resourceType();

            // Environment attribute used by the policy to compare with user_rentals.expires_at.
            // Reading it from DB keeps it consistent with PostgreSQL now().
            case "environment.current_time" -> readCurrentTime();

            default -> throw new IllegalArgumentException(
                    "Unsupported request attribute: " + context.attrKey()
            );
        };
    }

    private String readCurrentTime() {
        Timestamp ts = jdbcTemplate.queryForObject("""
            SELECT now()
        """, Timestamp.class);

        return ts == null ? null : ts.toLocalDateTime().toString();
    }
}
