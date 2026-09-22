package vn.ucon.demo.ucon.obligation;

public record FulfillObligationResult(
        String message,
        String obligationId,
        String version,
        String type,
        String scope,
        Long userId,
        String policyRef
) {
}