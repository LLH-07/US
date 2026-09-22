package vn.ucon.demo.ucon.pip.provider.dependency;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.pip.provider.DependencyResolver;

import java.util.ArrayList;
import java.util.List;

@Component
public class FamilyShareDependencyResolver implements DependencyResolver {

    private final JdbcTemplate jdbcTemplate;

    public FamilyShareDependencyResolver(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String dependencyName) {
        return "FamilyShareDependencyResolver".equals(dependencyName);
    }

    @Override
    public List<SessionDependency> resolve(
            String attrKey,
            AccessRequest request
    ) {
        return switch (attrKey) {
            case "family_share_active" -> resolveFamilyShareActive(attrKey, request);

            default -> throw new IllegalArgumentException(
                    "Unsupported family share dependency attribute: " + attrKey
            );
        };
    }

    private List<SessionDependency> resolveFamilyShareActive(
            String attrKey,
            AccessRequest request
    ) {
        List<FamilyShareContext> contexts = findActiveFamilyShareContexts(request);

        if (contexts.isEmpty()) {
            return List.of();
        }

        List<SessionDependency> dependencies = new ArrayList<>();

        for (FamilyShareContext context : contexts) {
            dependencies.addAll(List.of(
                    new SessionDependency(
                            "FAMILY_GROUP",
                            attrKey,
                            "FAMILY_GROUPS",
                            context.familyGroupId(),
                            "status",
                            null,
                            null
                    ),

                    new SessionDependency(
                            "FAMILY_GROUP_MEMBER",
                            attrKey,
                            "FAMILY_GROUP_MEMBERS",
                            context.familyGroupMemberId(),
                            "status",
                            null,
                            null
                    ),
                    new SessionDependency(
                            "FAMILY_GROUP_MEMBER",
                            attrKey,
                            "FAMILY_GROUP_MEMBERS",
                            context.familyGroupMemberId(),
                            "removed_at",
                            null,
                            null
                    ),

                    new SessionDependency(
                            "FAMILY_SHARED_VOLUME",
                            attrKey,
                            "FAMILY_SHARED_VOLUMES",
                            context.familySharedVolumeId(),
                            "status",
                            null,
                            null
                    ),
                    new SessionDependency(
                            "FAMILY_SHARED_VOLUME",
                            attrKey,
                            "FAMILY_SHARED_VOLUMES",
                            context.familySharedVolumeId(),
                            "revoked_at",
                            null,
                            null
                    )
            ));
        }

        return dependencies;
    }

    private List<FamilyShareContext> findActiveFamilyShareContexts(AccessRequest request) {
        return jdbcTemplate.query("""
            SELECT
                fg.id  AS family_group_id,
                fgm.id AS family_group_member_id,
                fsv.id AS family_shared_volume_id
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
        """, (rs, rowNum) -> new FamilyShareContext(
                rs.getLong("family_group_id"),
                rs.getLong("family_group_member_id"),
                rs.getLong("family_shared_volume_id")
        ), request.subjectUserId(), request.resourceId());
    }

    private record FamilyShareContext(
            Long familyGroupId,
            Long familyGroupMemberId,
            Long familySharedVolumeId
    ) {
    }
}