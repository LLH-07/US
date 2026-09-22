package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

@Component
public class UserAttributeReader implements AttributeReader {

    private final JdbcTemplate jdbcTemplate;

    public UserAttributeReader(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return "UserAttributeReader".equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        return switch (context.attrKey()) {
            // New schema-based attribute key
            case "users.status" -> readUserStatus(context);

            default -> throw new IllegalArgumentException(
                    "Unsupported user attribute: " + context.attrKey()
            );
        };
    }

    private String readUserStatus(AttributeReadContext context) {
        return jdbcTemplate.queryForObject("""
            SELECT status
            FROM users
            WHERE id = ?
        """, String.class, context.request().subjectUserId());
    }
}
