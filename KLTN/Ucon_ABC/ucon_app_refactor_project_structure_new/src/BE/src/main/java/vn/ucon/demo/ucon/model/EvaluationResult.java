package vn.ucon.demo.ucon.model;

import java.time.LocalDateTime;

public record EvaluationResult(
        PolicyMetadata policyMetadata,
        EvaluationContext evaluationContext,
        PolicyDecision policyDecision,
        LocalDateTime evaluatedAt
) {}