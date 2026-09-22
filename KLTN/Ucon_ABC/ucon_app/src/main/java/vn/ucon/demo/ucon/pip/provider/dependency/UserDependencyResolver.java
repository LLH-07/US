package vn.ucon.demo.ucon.pip.provider.dependency;

import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.pip.provider.DependencyResolver;

import java.util.List;

@Component
public class UserDependencyResolver implements DependencyResolver {

    @Override
    public boolean supports(String dependencyName) {
        return "UserDependencyResolver".equals(dependencyName);
    }

    @Override
    public List<SessionDependency> resolve(
            String attrKey,
            AccessRequest request
    ) {
        return switch (attrKey) {
            // New schema-based attribute key
            case "users.status" -> resolveUserStatus(attrKey, request);

            default -> throw new IllegalArgumentException(
                    "Unsupported user dependency attribute: " + attrKey
            );
        };
    }

    private List<SessionDependency> resolveUserStatus(
            String attrKey,
            AccessRequest request
    ) {
        return List.of(new SessionDependency(
                "USER",
                attrKey,
                "USERS",
                request.subjectUserId(),
                "status",
                null,
                null
        ));
    }
}
