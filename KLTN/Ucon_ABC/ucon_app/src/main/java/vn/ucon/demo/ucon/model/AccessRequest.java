package vn.ucon.demo.ucon.model;

public record AccessRequest(
        Long subjectUserId,
        String resourceType,
        Long resourceId,
        String actionId,
        String accessPath,
        String phase,
        Long usageSessionId
) {}