package vn.ucon.demo.ucon.model;

/**
 * Application-side definition used by the PIP to resolve an attribute-update
 * action code into a concrete mutable attribute and handler.
 */
public record AttributeUpdateDefinition(
        String actionCode,
        String attrKey,
        String handlerName,
        String operation,
        String defaultValue,
        boolean enabled
) {}
