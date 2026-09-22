package vn.ucon.demo.ucon.pdp;

import jakarta.annotation.PostConstruct;
import oasis.names.tc.xacml._3_0.core.schema.wd_17.DecisionType;
import org.ow2.authzforce.core.pdp.api.AttributeFqn;
import org.ow2.authzforce.core.pdp.api.AttributeFqns;
import org.ow2.authzforce.core.pdp.api.DecisionRequest;
import org.ow2.authzforce.core.pdp.api.DecisionRequestBuilder;
import org.ow2.authzforce.core.pdp.api.DecisionResult;
import org.ow2.authzforce.core.pdp.api.PepAction;
import org.ow2.authzforce.core.pdp.api.PepActionAttributeAssignment;
import org.ow2.authzforce.core.pdp.api.value.AttributeBag;
import org.ow2.authzforce.core.pdp.api.value.Bags;
import org.ow2.authzforce.core.pdp.api.value.BooleanValue;
import org.ow2.authzforce.core.pdp.api.value.StandardDatatypes;
import org.ow2.authzforce.core.pdp.api.value.StringValue;
import org.ow2.authzforce.core.pdp.api.value.DateTimeValue;
import org.ow2.authzforce.core.pdp.impl.BasePdpEngine;
import org.ow2.authzforce.core.pdp.impl.PdpEngineConfiguration;
import org.springframework.core.io.ClassPathResource;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.model.EvaluationContext;
import vn.ucon.demo.ucon.model.PolicyDecision;
import vn.ucon.demo.ucon.model.RequiredObligation;
import vn.ucon.demo.ucon.model.ResolvedAttribute;
import vn.ucon.demo.ucon.model.AttributeUpdateInstruction;


import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Component
public class AuthzForcePdpAdapter {

    private static final String XSD_STRING = "http://www.w3.org/2001/XMLSchema#string";
    private static final String XSD_BOOLEAN = "http://www.w3.org/2001/XMLSchema#boolean";
    private static final String XSD_DATETIME = "http://www.w3.org/2001/XMLSchema#dateTime";

    private static final String ATTRIBUTE_UPDATE_PEP_ACTION_ID =
            "urn:ucon:instruction:attribute-update";
    private static final String ATTRIBUTE_UPDATE_ACTION_CODE_ID =
            "urn:ucon:instruction:attribute-update:action-code";

    private static final String MISSING_PRE_OBLIGATION_ADVICE_ID =
        "urn:ucon:advice:missing-pre-obligation";

    private static final String OBLIGATION_ID_ASSIGNMENT =
            "urn:ucon:obligation:id";

    private static final String OBLIGATION_TYPE_ASSIGNMENT =
            "urn:ucon:obligation:type";

    private static final String OBLIGATION_VERSION_ASSIGNMENT =
            "urn:ucon:obligation:version";

    private static final String OBLIGATION_POLICY_REF_ASSIGNMENT =
            "urn:ucon:obligation:policy-ref";

    private BasePdpEngine pdp;

    @PostConstruct
    public void init() {
        try {
            File pdpXml = new ClassPathResource("authzforce/pdp.xml").getFile();
            PdpEngineConfiguration conf =
                    PdpEngineConfiguration.getInstance(pdpXml.toURI().toString());
            this.pdp = new BasePdpEngine(conf);
        } catch (Exception e) {
            throw new IllegalStateException("Cannot initialize AuthzForce embedded PDP", e);
        }
    }

    public PolicyDecision evaluate(EvaluationContext ctx) {
        try {
            DecisionRequestBuilder<?> builder = pdp.newRequestBuilder(-1, -1);

            for (ResolvedAttribute attr : ctx.attributes()) {
                putAttribute(builder, attr);
            }

            DecisionRequest request = builder.build(false);
            DecisionResult result = pdp.evaluate(request);

            boolean permit = result.getDecision() == DecisionType.PERMIT;
            
            List<AttributeUpdateInstruction> attributeUpdateInstructions =
                    extractAttributeUpdateInstructions(result);

            List<RequiredObligation> requiredObligations =
                    extractRequiredObligations(result);

            String reasonCode;

            if (permit) {
                reasonCode = "PERMIT";
            } else if (!requiredObligations.isEmpty()) {
                reasonCode = "PRE_OBLIGATION_REQUIRED";
            } else {
                reasonCode = "POLICY_NOT_SATISFIED";
            }

            return new PolicyDecision(
                permit,
                result.getDecision().value(),
                reasonCode,
                attributeUpdateInstructions,
                requiredObligations
            );
        } catch (Exception e) {
            return new PolicyDecision(
                false,
                "INDETERMINATE",
                e.getMessage(),
                List.of(),
                List.of()
            );
        }
    }

    private List<RequiredObligation> extractRequiredObligations(
            DecisionResult result
    ) {
        List<RequiredObligation> required = new ArrayList<>();

        for (PepAction pepAction : result.getPepActions()) {

            // Advice không phải mandatory action.
            if (pepAction.isMandatory()) {
                continue;
            }

            if (!MISSING_PRE_OBLIGATION_ADVICE_ID.equals(
                    pepAction.getId()
            )) {
                continue;
            }

            String obligationId = findAssignmentValue(
                    pepAction,
                    OBLIGATION_ID_ASSIGNMENT
            );

            String type = findAssignmentValue(
                    pepAction,
                    OBLIGATION_TYPE_ASSIGNMENT
            );

            String version = findAssignmentValue(
                    pepAction,
                    OBLIGATION_VERSION_ASSIGNMENT
            );

            String policyRef = findAssignmentValue(
                    pepAction,
                    OBLIGATION_POLICY_REF_ASSIGNMENT
            );

            if (obligationId == null || obligationId.isBlank()) {
                throw new IllegalStateException(
                        "Missing obligation ID in advice: "
                                + MISSING_PRE_OBLIGATION_ADVICE_ID
                );
            }

            required.add(new RequiredObligation(
                    obligationId,
                    type,
                    version,
                    policyRef
            ));
        }

        return required;
    }

    private void putAttribute(DecisionRequestBuilder<?> builder, ResolvedAttribute attr) {
        if (attr.value() == null) {
            return;
        }

        if (XSD_BOOLEAN.equals(attr.xacmlDataType())) {
            putBoolean(
                    builder,
                    attr.xacmlCategory(),
                    attr.xacmlAttributeId(),
                    Boolean.parseBoolean(String.valueOf(attr.value()))
            );
            return;
        }

        if (XSD_STRING.equals(attr.xacmlDataType())) {
            putString(
                    builder,
                    attr.xacmlCategory(),
                    attr.xacmlAttributeId(),
                    String.valueOf(attr.value())
            );
            return;
        }

        if (XSD_DATETIME.equals(attr.xacmlDataType())) {
            putDateTime(
                    builder,
                    attr.xacmlCategory(),
                    attr.xacmlAttributeId(),
                    String.valueOf(attr.value())
            );
            return;
        }

        throw new IllegalArgumentException(
                "Unsupported XACML data type for demo: " + attr.xacmlDataType()
        );
    }

    private void putString(
            DecisionRequestBuilder<?> builder,
            String category,
            String attributeId,
            String value
    ) {
        AttributeFqn fqn = AttributeFqns.newInstance(category, Optional.empty(), attributeId);

        AttributeBag<?> bag = Bags.singletonAttributeBag(
                StandardDatatypes.STRING,
                new StringValue(value)
        );

        builder.putNamedAttributeIfAbsent(fqn, bag);
    }

    private void putBoolean(
            DecisionRequestBuilder<?> builder,
            String category,
            String attributeId,
            boolean value
    ) {
        AttributeFqn fqn = AttributeFqns.newInstance(category, Optional.empty(), attributeId);

        AttributeBag<?> bag = Bags.singletonAttributeBag(
                StandardDatatypes.BOOLEAN,
                new BooleanValue(value)
        );

        builder.putNamedAttributeIfAbsent(fqn, bag);
    }

    private void putDateTime(
            DecisionRequestBuilder<?> builder,
            String category,
            String attributeId,
            String value
    ) {
        AttributeFqn fqn = AttributeFqns.newInstance(category, Optional.empty(), attributeId);

        AttributeBag<?> bag = Bags.singletonAttributeBag(
                StandardDatatypes.DATETIME,
                new DateTimeValue(value)
        );

        builder.putNamedAttributeIfAbsent(fqn, bag);
    }

    private List<AttributeUpdateInstruction> extractAttributeUpdateInstructions(
            DecisionResult result
    ) {
        List<AttributeUpdateInstruction> updates = new ArrayList<>();

        for (PepAction pepAction : result.getPepActions()) {
            if (!pepAction.isMandatory()) {
                continue;
            }

            if (!ATTRIBUTE_UPDATE_PEP_ACTION_ID.equals(pepAction.getId())) {
                continue;
            }

            String actionCode = findAssignmentValue(
                    pepAction,
                    ATTRIBUTE_UPDATE_ACTION_CODE_ID
            );

            if (actionCode == null || actionCode.isBlank()) {
                throw new IllegalStateException(
                        "Missing action code in attribute-update PEP action: "
                                + ATTRIBUTE_UPDATE_PEP_ACTION_ID
                );
            }

            updates.add(new AttributeUpdateInstruction(actionCode));
        }

        return updates;
    }

    private String findAssignmentValue(PepAction pepAction, String attributeId) {
        for (PepActionAttributeAssignment<?> assignment : pepAction.getAttributeAssignments()) {
            if (attributeId.equals(assignment.getAttributeId())) {
                return String.valueOf(assignment.getValue().toString()).trim();
            }
        }

        return null;
    }


}