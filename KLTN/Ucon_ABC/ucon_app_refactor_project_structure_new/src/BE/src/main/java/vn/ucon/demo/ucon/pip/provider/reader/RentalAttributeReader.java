package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

import java.sql.Timestamp;
import java.util.List;
import java.util.Optional;

@Component
public class RentalAttributeReader implements AttributeReader {

    private final JdbcTemplate jdbcTemplate;

    public RentalAttributeReader(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return "RentalAttributeReader".equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        return switch (context.attrKey()) {
            // New schema-based attribute keys
            case "user_rentals.status" -> readUserRentalStatus(context);
            case "user_rentals.expires_at" -> readUserRentalExpiresAt(context);

            default -> throw new IllegalArgumentException(
                    "Unsupported rental attribute: " + context.attrKey()
            );
        };
    }

    private String readUserRentalStatus(AttributeReadContext context) {
        Optional<Long> rentalId = findRentalId(context);

        if (rentalId.isEmpty()) {
            return null;
        }

        return jdbcTemplate.queryForObject("""
            SELECT status
            FROM user_rentals
            WHERE id = ?
        """, String.class, rentalId.get());
    }

    private String readUserRentalExpiresAt(AttributeReadContext context) {
        Optional<Long> rentalId = findRentalId(context);

        if (rentalId.isEmpty()) {
            return null;
        }

        Timestamp ts = jdbcTemplate.queryForObject("""
            SELECT expires_at
            FROM user_rentals
            WHERE id = ?
        """, Timestamp.class, rentalId.get());

        return toXacmlDateTime(ts);
    }

    private Optional<Long> findRentalId(AttributeReadContext context) {
        Optional<Long> dependencyRentalId = findCurrentRentalDependency(context.dependencies());

        if (dependencyRentalId.isPresent()) {
            return dependencyRentalId;
        }

        return findBestRentalForSubject(context.request().subjectUserId());
    }

    private Optional<Long> findCurrentRentalDependency(List<SessionDependency> dependencies) {
        return dependencies.stream()
                .filter(dep -> "CURRENT_RENTAL".equals(dep.dependencyKey()))
                .map(SessionDependency::entityId)
                .findFirst();
    }

    private Optional<Long> findBestRentalForSubject(Long subjectUserId) {
        List<Long> rentalIds = jdbcTemplate.queryForList("""
            SELECT id
            FROM user_rentals
            WHERE user_id = ?
            ORDER BY
                CASE
                    WHEN status = 'ACTIVE' AND expires_at > now() THEN 0
                    ELSE 1
                END,
                expires_at DESC
            LIMIT 1
        """, Long.class, subjectUserId);

        return rentalIds.stream().findFirst();
    }

    private String toXacmlDateTime(Timestamp ts) {
        return ts == null ? null : ts.toLocalDateTime().toString();
    }
}
