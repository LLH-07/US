package vn.ucon.demo.ucon.model;

import java.util.List;

public record AccessDecision(
        boolean permit,
        String rawDecision,
        String reasonCode,
        Long usageSessionId,
        String selectedAccessPath,
        List<AccessEvaluationAttempt> attempts
) {
        public AccessDecision {
                attempts = attempts == null
                        ? List.of()
                        : List.copyOf(attempts);
        }

        public static AccessDecision granted(
                String rawDecision,
                String reasonCode,
                Long usageSessionId,
                String selectedAccessPath,
                List<AccessEvaluationAttempt> attempts
        ) {
                return new AccessDecision(
                        true,
                        rawDecision,
                        reasonCode,
                        usageSessionId,
                        selectedAccessPath,
                        attempts
                );
        }

        public static AccessDecision denied(
                String rawDecision,
                String reasonCode,
                Long usageSessionId
        ) {
                return new AccessDecision(
                        false,
                        rawDecision,
                        reasonCode,
                        usageSessionId,
                        null,
                        List.of()
                );
        }

        public static AccessDecision denied(
                String rawDecision,
                String reasonCode,
                List<AccessEvaluationAttempt> attempts
        ) {
                return new AccessDecision(
                        false,
                        rawDecision,
                        reasonCode,
                        null,
                        null,
                        attempts
                );
        }
}