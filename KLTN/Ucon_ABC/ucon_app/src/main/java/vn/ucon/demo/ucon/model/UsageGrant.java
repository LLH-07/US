package vn.ucon.demo.ucon.model;

import java.util.Map;

public record UsageGrant(
        boolean permit,
        String decision,
        String reasonCode,
        Long usageSessionId,
        String resourceType,
        Long resourceId,
        String actionId,
        String accessPath
) {
    public static UsageGrant deny(String decision, String reasonCode, Long usageSessionId) {
        return new UsageGrant(
                false,
                decision,
                reasonCode,
                usageSessionId,
                null,
                null,
                null,
                null
        );
    }

    public static UsageGrant permit(
            String decision,
            String reasonCode,
            Long usageSessionId,
            Map<String, Object> session
    ) {
        return new UsageGrant(
                true,
                decision,
                reasonCode,
                usageSessionId,
                (String) session.get("resource_type"),
                ((Number) session.get("resource_id")).longValue(),
                (String) session.get("action_id"),
                (String) session.get("access_path")
        );
    }
}