package vn.ucon.demo.ucon.model;

import java.util.List;

public record AccessEvaluationAttempt(
        String accessPath,
        String result,
        String rawDecision,
        String reasonCode,
        String policyRef,
        String message,
        List<RequiredObligation> requiredObligations
) {
        public AccessEvaluationAttempt {
                requiredObligations = requiredObligations == null
                        ? List.of()
                        : List.copyOf(requiredObligations);
        }

        public static AccessEvaluationAttempt skipped(
                String accessPath,
                String reasonCode,
                String message
        ) {
                return new AccessEvaluationAttempt(
                        accessPath,
                        "SKIPPED",
                        null,
                        reasonCode,
                        null,
                        message,
                        List.of()
                );
        }

        public static AccessEvaluationAttempt skipped(
                String accessPath,
                PolicyMetadata policy,
                String reasonCode,
                String message
        ) {
                return new AccessEvaluationAttempt(
                        accessPath,
                        "SKIPPED",
                        null,
                        reasonCode,
                        policy != null ? policy.policyRef() : null,
                        message,
                        List.of()
                );
        }

        public static AccessEvaluationAttempt evaluated(
                String accessPath,
                PolicyMetadata policy,
                PolicyDecision decision,
                List<RequiredObligation> requiredObligations
        ) {
                return new AccessEvaluationAttempt(
                        accessPath,
                        decision.permit() ? "PERMIT" : "DENY",
                        decision.rawDecision(),
                        decision.reasonCode(),
                        policy.policyRef(),
                        null,
                        requiredObligations
                );
        }
}