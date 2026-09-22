package vn.ucon.demo.ucon.model;

public record AccessDecision(
        boolean permit,
        String rawDecision,
        String reasonCode,
        Long usageSessionId
) {}