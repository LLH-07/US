package vn.ucon.demo.ucon.pip.provider;

/**
 * Request from the Context Handler to the PIP to apply one policy-directed
 * attribute update for a usage session.
 */
public record AttributeUpdateRequest(
        String actionCode,
        Long usageSessionId,
        Object value
) {}
