package vn.ucon.demo.ucon.model;

public record PolicyAttributeRequirement(
        Long id,
        Long policyId,
        String attrKey,
        String usageType,
        boolean required
) {
    public boolean isEvaluationAttribute() {
        return "EVALUATION".equalsIgnoreCase(usageType);
    }
}