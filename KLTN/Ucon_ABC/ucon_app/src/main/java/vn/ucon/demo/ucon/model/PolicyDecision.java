package vn.ucon.demo.ucon.model;

import java.util.List;

public record PolicyDecision(
        boolean permit,
        String rawDecision,
        String reasonCode,
        List<EnforcementInstruction> instructions
) {
    public PolicyDecision(
            boolean permit,
            String rawDecision,
            String reasonCode
    ) {
        this(permit, rawDecision, reasonCode, List.of());
    }
}