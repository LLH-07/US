package vn.ucon.demo.ucon.pip.provider;

public record AttributeUpdateCommand(
        String attrKey,
        String operation,
        Object value,
        String timing,
        Long usageSessionId
) {}
