package vn.ucon.demo.ucon.pip.provider.dependency;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.pip.provider.DependencyResolver;

import java.util.List;

@Component
public class EbookResourceDependencyResolver implements DependencyResolver {

    private final JdbcTemplate jdbcTemplate;

    public EbookResourceDependencyResolver(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String dependencyName) {
        return "EbookResourceDependencyResolver".equals(dependencyName);
    }

    @Override
    public List<SessionDependency> resolve(
            String attrKey,
            AccessRequest request
    ) {
        return switch (attrKey) {
            // New schema-based attribute keys
            case "book_volumes.status" -> resolveBookVolumeStatus(attrKey, request);
            case "books.status" -> resolveBookStatus(attrKey, request);
            case "book_volumes.access_model_override" -> resolveBookVolumeAccessModelOverride(attrKey, request);

            default -> throw new IllegalArgumentException(
                    "Unsupported ebook resource dependency attribute: " + attrKey
            );
        };
    }

    private List<SessionDependency> resolveBookVolumeStatus(
            String attrKey,
            AccessRequest request
    ) {
        return List.of(new SessionDependency(
                "BOOK_VOLUME",
                attrKey,
                "BOOK_VOLUMES",
                request.resourceId(),
                "status",
                null,
                null
        ));
    }

    private List<SessionDependency> resolveBookStatus(
            String attrKey,
            AccessRequest request
    ) {
        Long bookId = findBookIdOfResource(request);

        return List.of(new SessionDependency(
                "BOOK",
                attrKey,
                "BOOKS",
                bookId,
                "status",
                null,
                null
        ));
    }

    private List<SessionDependency> resolveBookVolumeAccessModelOverride(
            String attrKey,
            AccessRequest request
    ) {
        return List.of(new SessionDependency(
                "BOOK_VOLUME",
                attrKey,
                "BOOK_VOLUMES",
                request.resourceId(),
                "access_model_override",
                null,
                null
        ));
    }

    private Long findBookIdOfResource(AccessRequest request) {
        return jdbcTemplate.queryForObject("""
            SELECT book_id
            FROM book_volumes
            WHERE id = ?
        """, Long.class, request.resourceId());
    }
}
