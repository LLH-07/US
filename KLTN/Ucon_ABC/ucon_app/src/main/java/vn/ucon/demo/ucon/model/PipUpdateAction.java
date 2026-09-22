package vn.ucon.demo.ucon.model;

public record PipUpdateAction(
        String actionCode,
        String attrKey,
        String handlerName,
        String operation,
        String defaultValue,
        boolean enabled
) {}