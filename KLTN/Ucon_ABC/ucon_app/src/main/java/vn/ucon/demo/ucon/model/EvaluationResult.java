package vn.ucon.demo.ucon.model;

public record EvaluationResult(
        PolicyMetadata policyMetadata,
        EvaluationContext evaluationContext,
        PolicyDecision policyDecision
) {}