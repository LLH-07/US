package vn.ucon.demo.ucon.pip.provider;

/**
 * Fully resolved attribute update passed by the PIP to an application-specific
 * update handler.
 */
public record ResolvedAttributeUpdate(
        String actionCode,
        String attrKey,
        String operation,
        Object value,
        Long usageSessionId
) {}
