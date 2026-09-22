package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

@Component
public class FamilyShareAttributeReader implements AttributeReader {

    private final JdbcTemplate jdbcTemplate;

    public FamilyShareAttributeReader(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return "FamilyShareAttributeReader".equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        return switch (context.attrKey()) {
            case "family_share_active" -> readFamilyShareActive(context);

            default -> throw new IllegalArgumentException(
                    "Unsupported family share attribute: " + context.attrKey()
            );
        };
    }

    private boolean readFamilyShareActive(AttributeReadContext context) {
        Boolean active = jdbcTemplate.queryForObject("""
            SELECT EXISTS (
                SELECT 1
                FROM family_group_members fgm
                JOIN family_groups fg
                    ON fg.id = fgm.group_id
                JOIN family_shared_volumes fsv
                    ON fsv.group_id = fg.id
                WHERE fgm.user_id = ?
                  AND fsv.volume_id = ?
                  AND fg.status = 'ACTIVE'
                  AND fgm.status = 'ACTIVE'
                  AND fgm.removed_at IS NULL
                  AND fsv.status = 'ACTIVE'
                  AND fsv.revoked_at IS NULL
            )
        """, Boolean.class,
                context.request().subjectUserId(),
                context.request().resourceId()
        );

        return Boolean.TRUE.equals(active);
    }
}