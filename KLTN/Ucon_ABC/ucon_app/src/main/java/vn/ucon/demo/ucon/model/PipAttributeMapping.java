package vn.ucon.demo.ucon.model;

public record PipAttributeMapping(
        String attrKey,
        String xacmlAttributeId,
        String xacmlCategory,
        String xacmlDataType,
        String readerName,
        String writerName,
        String dependencyName,
        String monitorType,
        boolean mutable,
        boolean enabled
) {}