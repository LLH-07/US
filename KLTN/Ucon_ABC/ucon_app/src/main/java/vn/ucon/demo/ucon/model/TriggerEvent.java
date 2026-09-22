package vn.ucon.demo.ucon.model;

public record TriggerEvent(
        Long id,
        String sourceType,
        String entityType,
        Long entityId,
        String attributeName,
        String eventType
) {}