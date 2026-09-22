package vn.ucon.demo.ucon.model;

public record ResolvedAttribute(
        String attributeName,
        String xacmlAttributeId,
        String xacmlCategory,
        String xacmlDataType,
        Object value
) {}