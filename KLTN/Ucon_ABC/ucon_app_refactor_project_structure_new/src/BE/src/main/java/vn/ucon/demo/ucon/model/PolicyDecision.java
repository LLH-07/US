package vn.ucon.demo.ucon.model;

import java.util.List;

public record PolicyDecision(
        boolean permit,
        String rawDecision,
        String reasonCode,
        List<AttributeUpdateInstruction> attributeUpdateInstructions,
        List<RequiredObligation> requiredObligations
) {

    public PolicyDecision {
        attributeUpdateInstructions = attributeUpdateInstructions == null
                ? List.of()
                : List.copyOf(attributeUpdateInstructions);

        requiredObligations = requiredObligations == null
                ? List.of()
                : List.copyOf(requiredObligations);
    }

    public PolicyDecision(
            boolean permit,
            String rawDecision,
            String reasonCode
    ) {
        this(
                permit,
                rawDecision,
                reasonCode,
                List.of(),
                List.of()
        );
    }
}
