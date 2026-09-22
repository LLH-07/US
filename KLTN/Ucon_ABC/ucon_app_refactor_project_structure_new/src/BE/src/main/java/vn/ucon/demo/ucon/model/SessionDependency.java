package vn.ucon.demo.ucon.model;

import java.time.LocalDateTime;

public record SessionDependency(
        String dependencyKey,
        String attrKey,
        String entityType,
        Long entityId,
        String attributeName,
        String monitorType,
        LocalDateTime nextCheckAt
) {
}