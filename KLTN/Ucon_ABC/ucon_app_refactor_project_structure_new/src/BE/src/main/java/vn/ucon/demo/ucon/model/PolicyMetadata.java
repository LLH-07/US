package vn.ucon.demo.ucon.model;

import java.util.List;

public record PolicyMetadata(
        Long policyId,
        String policyRef,
        String policyName,
        String resourceType,
        String actionId,
        String accessPath,
        String phase,
        List<PolicyAttributeRequirement> attributeRequirements
) {
    public List<PolicyAttributeRequirement> evaluationAttributes() {
        return attributeRequirements.stream()
                .filter(PolicyAttributeRequirement::isEvaluationAttribute)
                .toList();
    }

    public List<PolicyAttributeRequirement> ongoingDependencyAttributes() {
        return evaluationAttributes();
    }
}