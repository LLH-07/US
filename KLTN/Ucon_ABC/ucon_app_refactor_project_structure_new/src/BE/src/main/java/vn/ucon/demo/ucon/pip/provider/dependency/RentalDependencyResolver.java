package vn.ucon.demo.ucon.pip.provider.dependency;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.pip.provider.DependencyResolver;

import java.time.LocalDateTime;
import java.util.List;

@Component
public class RentalDependencyResolver implements DependencyResolver {

    private final JdbcTemplate jdbcTemplate;

    public RentalDependencyResolver(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String dependencyName) {
        return "RentalDependencyResolver".equals(dependencyName);
    }

    @Override
    public List<SessionDependency> resolve(
            String attrKey,
            AccessRequest request
    ) {
        return switch (attrKey) {
            // New schema-based attribute keys
            case "user_rentals.status" -> resolveUserRentalStatus(attrKey, request);
            case "user_rentals.expires_at" -> resolveUserRentalExpiresAt(attrKey, request);

            default -> throw new IllegalArgumentException(
                    "Unsupported rental dependency attribute: " + attrKey
            );
        };
    }

    private List<SessionDependency> resolveUserRentalStatus(
            String attrKey,
            AccessRequest request
    ) {
        RentalRow rental = findCurrentRental(request);

        if (rental == null) {
            return List.of();
        }

        return List.of(new SessionDependency(
                "USER_RENTAL",
                attrKey,
                "USER_RENTALS",
                rental.id(),
                "status",
                null,
                null
        ));
    }

    private List<SessionDependency> resolveUserRentalExpiresAt(
            String attrKey,
            AccessRequest request
    ) {
        RentalRow rental = findCurrentRental(request);

        if (rental == null) {
            return List.of();
        }

        return List.of(new SessionDependency(
                "USER_RENTAL",
                attrKey,
                "USER_RENTALS",
                rental.id(),
                "expires_at",
                null,
                rental.expiresAt()
        ));
    }

    private RentalRow findCurrentRental(AccessRequest request) {
        List<RentalRow> rows = jdbcTemplate.query("""
            SELECT id, expires_at
            FROM user_rentals
            WHERE user_id = ?
              AND status = 'ACTIVE'
            ORDER BY expires_at DESC
            LIMIT 1
        """, (rs, rowNum) -> new RentalRow(
                rs.getLong("id"),
                rs.getTimestamp("expires_at").toLocalDateTime()
        ), request.subjectUserId());

        return rows.isEmpty() ? null : rows.get(0);
    }

    private record RentalRow(
            Long id,
            LocalDateTime expiresAt
    ) {
    }
}
