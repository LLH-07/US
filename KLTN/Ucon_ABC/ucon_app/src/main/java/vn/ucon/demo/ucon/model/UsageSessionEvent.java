package vn.ucon.demo.ucon.model;

public record UsageSessionEvent(
        UsageSessionEventType type,
        Long usageSessionId,
        String reasonCode
) {
    public static UsageSessionEvent revoked(Long usageSessionId, String reasonCode) {
        return new UsageSessionEvent(
                UsageSessionEventType.REVOKED,
                usageSessionId,
                reasonCode
        );
    }

    public static UsageSessionEvent ended(Long usageSessionId, String reasonCode) {
        return new UsageSessionEvent(
                UsageSessionEventType.ENDED,
                usageSessionId,
                reasonCode
        );
    }
}