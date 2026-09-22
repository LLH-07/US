package vn.ucon.demo.ucon.pip.provider;

public record AttributeUpdateActionCommand(
        String actionCode,
        Long usageSessionId,
        Object value
) {}