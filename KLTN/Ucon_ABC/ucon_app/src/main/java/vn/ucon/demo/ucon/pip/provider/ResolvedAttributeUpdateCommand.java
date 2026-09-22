package vn.ucon.demo.ucon.pip.provider;

public record ResolvedAttributeUpdateCommand(
        String actionCode,
        String attrKey,
        String operation,
        Object value,
        Long usageSessionId
) {}