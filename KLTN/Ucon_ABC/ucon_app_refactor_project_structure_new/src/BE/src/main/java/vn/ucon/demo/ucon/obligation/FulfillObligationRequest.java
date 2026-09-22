package vn.ucon.demo.ucon.obligation;

import java.util.Map;

public record FulfillObligationRequest(
        Long userId,
        String policyRef,
        Map<String, Object> evidence
) {
}