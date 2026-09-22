package vn.ucon.demo.ucon.ch;

import java.util.List;
import java.util.Map;

import org.springframework.context.ApplicationEventPublisher;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import vn.ucon.demo.ucon.model.AccessDecision;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.EnforcementInstruction;
import vn.ucon.demo.ucon.model.EvaluationContext;
import vn.ucon.demo.ucon.model.EvaluationResult;
import vn.ucon.demo.ucon.model.MonitorType;
import vn.ucon.demo.ucon.model.PolicyDecision;
import vn.ucon.demo.ucon.model.PolicyMetadata;
import vn.ucon.demo.ucon.model.ResolvedAttribute;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.model.TriggerEvent;
import vn.ucon.demo.ucon.model.UsageGrant;
import vn.ucon.demo.ucon.model.UsageSessionEvent;
import vn.ucon.demo.ucon.pap.PapService;
import vn.ucon.demo.ucon.pdp.AuthzForcePdpAdapter;
import vn.ucon.demo.ucon.pip.PipService;
import vn.ucon.demo.ucon.pip.provider.AttributeUpdateActionCommand;
import vn.ucon.demo.ucon.sm.UsageSessionManager;

@Component
public class ContextHandler {

        private static final String PHASE_PRE = "PRE";
        private static final String PHASE_ONGOING = "ONGOING";
        private static final String PHASE_POST = "POST";

        private static final String RAW_DENY = "DENY";
        private static final String RAW_PERMIT = "PERMIT";

        private static final String SOURCE_SCHEDULER = "SCHEDULER";
        private static final String SOURCE_DB_TRIGGER = "DB_TRIGGER";

        private static final String EVENT_TIMER_DUE = "TIMER_DUE";
        private static final String EVENT_ATTRIBUTE_CHANGED = "ATTRIBUTE_CHANGED";
        private static final String EVENT_ENTITY_DELETED = "ENTITY_DELETED";

        private static final String REASON_ENTITY_DELETED = "ENTITY_DELETED";

        private static final String OBLIGATION_UPDATE_ACTION = "urn:ucon:obligation:update-action";

        private final PapService papService;
        private final PipService pipService;
        private final AuthzForcePdpAdapter pdpAdapter;
        private final UsageSessionManager sessionManager;
        private final ApplicationEventPublisher eventPublisher;

        public ContextHandler(
                PapService papService,
                PipService pipService,
                AuthzForcePdpAdapter pdpAdapter,
                UsageSessionManager sessionManager,
                ApplicationEventPublisher eventPublisher
        ) {
                this.papService = papService;
                this.pipService = pipService;
                this.pdpAdapter = pdpAdapter;
                this.sessionManager = sessionManager;
                this.eventPublisher = eventPublisher;
        }

        @Transactional
        public AccessDecision handleTryAccess(AccessRequest request) {
                requirePhase(request, PHASE_PRE, "handleTryAccess");

                EvaluationResult preResult = evaluate(request, List.of());
                PolicyDecision preDecision = preResult.policyDecision();

                if (!preDecision.permit()) {
                        logEvaluation(
                                null,
                                "DENY_ACCESS",
                                PHASE_PRE,
                                preResult,
                                "PRE allowed condition not satisfied"
                        );

                        return toDecision(preDecision, null);
                }

                PolicyMetadata ongoingPolicy = findPolicy(request, PHASE_ONGOING);
                PolicyMetadata postPolicy = findPolicy(request, PHASE_POST);

                List<SessionDependency> dependencies = resolveDependencies(
                        request,
                        ongoingPolicy
                );

                Long sessionId = sessionManager.createPendingSession(
                        request,
                        List.of(preResult.policyMetadata(), ongoingPolicy, postPolicy),
                        dependencies,
                        preDecision
                );

                logEvaluation(
                        sessionId,
                        "CHECK_PRE_ACCESS",
                        PHASE_PRE,
                        preResult,
                        "CH evaluated PRE policy before creating usage session"
                );

                return toDecision(preDecision, sessionId);
        }

        @Transactional
        public AccessDecision handleStartAccess(Long sessionId) {
                if (sessionManager.isActive(sessionId)) {
                        return new AccessDecision(
                                true,
                                RAW_PERMIT,
                                "SESSION_ALREADY_ACTIVE",
                                sessionId
                        );
                }

                if (!sessionManager.isPending(sessionId)) {
                        return deny("SESSION_NOT_STARTABLE", sessionId);
                }

                EvaluationResult startResult = evaluateSession(
                        sessionId,
                        PHASE_ONGOING
                );
                PolicyDecision decision = startResult.policyDecision();

                logEvaluation(
                        sessionId,
                        "CHECK_START_ACCESS",
                        PHASE_ONGOING,
                        startResult,
                        "CH checked ongoing policy before usage starts"
                );

                if (!decision.permit()) {
                        sessionManager.denyPendingSession(
                                sessionId,
                                decision.reasonCode()
                        );

                        return toDecision(decision, sessionId);
                }

                sessionManager.activateSession(sessionId);

                return toDecision(decision, sessionId);
        }

        /**
         * Fast authorization path for resource delivery.
         *
         * This method does not re-evaluate ONGOING policy for active sessions.
         * ONGOING re-evaluation is triggered by Event Listener / Scheduler via handleTrigger().
         */
        @Transactional
        public UsageGrant authorizeUsage(Long sessionId) {
                if (sessionManager.isPending(sessionId)) {
                        return toGrant(handleStartAccess(sessionId), sessionId);
                }

                if (!sessionManager.isActive(sessionId)) {
                        return UsageGrant.deny(
                                RAW_DENY,
                                "SESSION_NOT_ACTIVE_OR_REVOKED",
                                sessionId
                        );
                }

                return UsageGrant.permit(
                        RAW_PERMIT,
                        "SESSION_ACTIVE",
                        sessionId,
                        sessionManager.findSession(sessionId)
                );
        }

        @Transactional
        public void handleTrigger(TriggerEvent triggerEvent) {
                MonitorType triggerMonitorType = resolveTriggerMonitorType(triggerEvent);

                List<Long> affectedSessionIds = sessionManager.findAffectedActiveSessions(
                        triggerEvent.entityType(),
                        triggerEvent.entityId(),
                        triggerEvent.attributeName(),
                        triggerMonitorType
                );

                if (isEntityDeleted(triggerEvent)) {
                        affectedSessionIds.forEach(
                                sessionId -> handleEntityDeleted(sessionId, triggerEvent)
                        );
                        return;
                }

                affectedSessionIds.forEach(this::reevaluate);
        }

        @Transactional
        public AccessDecision handleEndAccess(Long sessionId) {
                if (!sessionManager.isActive(sessionId)) {
                        return deny("SESSION_NOT_ACTIVE", sessionId);
                }

                sessionManager.endSession(sessionId);
                executePostUpdates(sessionId, "USER_END", null);

                eventPublisher.publishEvent(
                        UsageSessionEvent.ended(sessionId, "USER_END")
                );

                return new AccessDecision(
                        true,
                        RAW_PERMIT,
                        "USER_END",
                        sessionId
                );
        }

        private MonitorType resolveTriggerMonitorType(TriggerEvent triggerEvent) {
                if (SOURCE_SCHEDULER.equalsIgnoreCase(triggerEvent.sourceType())
                        || EVENT_TIMER_DUE.equalsIgnoreCase(triggerEvent.eventType())) {
                        return MonitorType.TIMER;
                }

                if (SOURCE_DB_TRIGGER.equalsIgnoreCase(triggerEvent.sourceType())
                        || EVENT_ATTRIBUTE_CHANGED.equalsIgnoreCase(triggerEvent.eventType())
                        || EVENT_ENTITY_DELETED.equalsIgnoreCase(triggerEvent.eventType())) {
                        return MonitorType.DB_EVENT;
                }

                throw new IllegalArgumentException(
                        "Unsupported trigger event source/type: "
                                + triggerEvent.sourceType()
                                + " / "
                                + triggerEvent.eventType()
                );
        }

        private boolean isEntityDeleted(TriggerEvent triggerEvent) {
                return EVENT_ENTITY_DELETED.equalsIgnoreCase(triggerEvent.eventType());
        }

        private void handleEntityDeleted(Long sessionId, TriggerEvent triggerEvent) {
                if (!sessionManager.isActive(sessionId)) {
                        return;
                }

                String reasonCode = entityDeletedReason(triggerEvent);

                sessionManager.logEvent(
                        sessionId,
                        "ENTITY_DELETED_TRIGGER",
                        PHASE_ONGOING,
                        RAW_DENY,
                        reasonCode,
                        jsonMessage(
                                "CH received ENTITY_DELETED trigger for "
                                        + triggerEvent.entityType()
                                        + "#"
                                        + triggerEvent.entityId()
                        )
                );

                revoke(sessionId, reasonCode, triggerEvent);
        }

        private String entityDeletedReason(TriggerEvent triggerEvent) {
                return REASON_ENTITY_DELETED
                        + ":"
                        + triggerEvent.entityType()
                        + ":"
                        + triggerEvent.entityId();
        }

        private void reevaluate(Long sessionId) {
                if (!sessionManager.isActive(sessionId)) {
                        return;
                }

                PolicyDecision decision = evalOngoing(
                        sessionId,
                        "EVENT_REEVALUATE",
                        "CH re-evaluated ongoing policy after trigger event"
                );

                if (!decision.permit()) {
                        revoke(sessionId, decision.reasonCode());
                }
        }

        private PolicyDecision evalOngoing(
                Long sessionId,
                String eventType,
                String message
        ) {
                long startedAt = System.nanoTime();

                EvaluationResult ongoingResult = evaluateSession(
                        sessionId,
                        PHASE_ONGOING
                );
                PolicyDecision decision = ongoingResult.policyDecision();

                long durationMs = (System.nanoTime() - startedAt) / 1_000_000;

                sessionManager.logEvent(
                        sessionId,
                        eventType,
                        PHASE_ONGOING,
                        decision.rawDecision(),
                        decision.reasonCode(),
                        evaluationDetailsJson(
                                ongoingResult,
                                message,
                                durationMs
                        )
                );

                return decision;
        }

        private EvaluationResult evaluateSession(Long sessionId, String phase) {
                AccessRequest request = requestFromSession(sessionId, phase);
                List<SessionDependency> dependencies = sessionManager.findDependencies(sessionId);

                return evaluate(request, dependencies);
        }

        private EvaluationResult evaluate(
                AccessRequest request,
                List<SessionDependency> dependencies
        ) {
                PolicyMetadata policyMetadata = findPolicy(request, request.phase());

                List<ResolvedAttribute> attributes = pipService.resolveAttributes(
                        request,
                        dependencies,
                        policyMetadata.evaluationAttributes()
                );

                EvaluationContext context = buildContext(
                        request,
                        policyMetadata,
                        dependencies,
                        attributes
                );

                PolicyDecision decision = pdpAdapter.evaluate(context);

                return new EvaluationResult(
                        policyMetadata,
                        context,
                        decision
                );
        }

        private PolicyMetadata findPolicy(AccessRequest request, String phase) {
                return papService.findPolicyMetadata(
                        request.resourceType(),
                        request.actionId(),
                        request.accessPath(),
                        phase
                );
        }

        private EvaluationContext buildContext(
                AccessRequest request,
                PolicyMetadata policyMetadata,
                List<SessionDependency> dependencies,
                List<ResolvedAttribute> attributes
        ) {
                return new EvaluationContext(
                        request,
                        policyMetadata,
                        dependencies,
                        attributes
                );
        }

        private List<SessionDependency> resolveDependencies(
                AccessRequest preRequest,
                PolicyMetadata ongoingPolicy
        ) {
                AccessRequest ongoingRequest = new AccessRequest(
                        preRequest.subjectUserId(),
                        preRequest.resourceType(),
                        preRequest.resourceId(),
                        preRequest.actionId(),
                        preRequest.accessPath(),
                        PHASE_ONGOING,
                        null
                );

                return pipService.resolveDependencies(
                        ongoingRequest,
                        ongoingPolicy.ongoingDependencyAttributes()
                );
        }

        private AccessRequest requestFromSession(Long sessionId, String phase) {
                Map<String, Object> session = sessionManager.findSession(sessionId);

                return new AccessRequest(
                        asLong(session, "subject_user_id"),
                        asString(session, "resource_type"),
                        asLong(session, "resource_id"),
                        asString(session, "action_id"),
                        asString(session, "access_path"),
                        phase,
                        sessionId
                );
        }

        private void revoke(Long sessionId, String reasonCode) {
                revoke(sessionId, reasonCode, null);
        }

        private void revoke(
                Long sessionId,
                String reasonCode,
                TriggerEvent triggerEvent
        ) {
                if (!sessionManager.isActive(sessionId)) {
                        return;
                }

                sessionManager.revokeSession(sessionId, reasonCode);
                executePostUpdates(sessionId, "REVOKE:" + reasonCode, triggerEvent);
                publishRevoked(sessionId, reasonCode);
        }

        private void publishRevoked(Long sessionId, String reasonCode) {
                eventPublisher.publishEvent(
                        UsageSessionEvent.revoked(sessionId, reasonCode)
                );

                sessionManager.logEvent(
                        sessionId,
                        "PEP_REVOKE_SIGNAL_SENT",
                        PHASE_ONGOING,
                        RAW_DENY,
                        reasonCode,
                        jsonMessage("CH published revoke signal for PEP")
                );
        }

        private void executePostUpdates(
                Long sessionId,
                String cause,
                TriggerEvent triggerEvent
        ) {
                if (sessionManager.isPostHandled(sessionId)) {
                        sessionManager.logEvent(
                                sessionId,
                                "POST_UPDATE_SKIPPED",
                                PHASE_POST,
                                RAW_PERMIT,
                                cause,
                                jsonMessage("Post-update already handled for this session")
                        );

                        return;
                }

                EvaluationResult postResult = evaluateSession(sessionId, PHASE_POST);
                PolicyDecision postDecision = postResult.policyDecision();

                sessionManager.logEvent(
                        sessionId,
                        "POST_UPDATE_COLLECTED",
                        PHASE_POST,
                        postDecision.rawDecision(),
                        cause,
                        evaluationDetailsJson(
                                postResult,
                                "CH collected post-update instructions from POST policy"
                        )
                );

                executeInstructions(sessionId, postDecision.instructions(), triggerEvent);
                sessionManager.markPostHandled(sessionId);
        }

        private void executeInstructions(
                Long sessionId,
                List<EnforcementInstruction> instructions,
                TriggerEvent triggerEvent
        ) {
                instructions.stream()
                        .filter(this::isUpdateAction)
                        .forEach(instruction -> executeUpdateAction(
                                sessionId,
                                instruction,
                                triggerEvent
                        ));
        }

        private boolean isUpdateAction(EnforcementInstruction instruction) {
                return OBLIGATION_UPDATE_ACTION.equals(instruction.obligationId());
        }

        private void executeUpdateAction(
                Long sessionId,
                EnforcementInstruction instruction,
                TriggerEvent triggerEvent
        ) {
                boolean executed = pipService.executeUpdateAction(
                        new AttributeUpdateActionCommand(
                                instruction.actionCode(),
                                sessionId,
                                null
                        ),
                        triggerEvent
                );

                if (!executed) {
                        sessionManager.logEvent(
                                sessionId,
                                "ATTRIBUTE_UPDATE_SKIPPED",
                                PHASE_POST,
                                RAW_DENY,
                                instruction.actionCode(),
                                jsonMessage("PIP skipped update action because target entity was deleted")
                        );
                        return;
                }

                sessionManager.logEvent(
                        sessionId,
                        "ATTRIBUTE_UPDATE",
                        PHASE_POST,
                        RAW_PERMIT,
                        instruction.actionCode(),
                        jsonMessage("PIP update action executed")
                );
        }

        private UsageGrant toGrant(AccessDecision decision, Long sessionId) {
                if (!decision.permit()) {
                        return UsageGrant.deny(
                                decision.rawDecision(),
                                decision.reasonCode(),
                                sessionId
                        );
                }

                return UsageGrant.permit(
                        decision.rawDecision(),
                        decision.reasonCode(),
                        sessionId,
                        sessionManager.findSession(sessionId)
                );
        }

        private AccessDecision toDecision(PolicyDecision decision, Long sessionId) {
                return new AccessDecision(
                        decision.permit(),
                        decision.rawDecision(),
                        decision.reasonCode(),
                        sessionId
                );
        }

        private AccessDecision deny(String reasonCode, Long sessionId) {
                return new AccessDecision(
                        false,
                        RAW_DENY,
                        reasonCode,
                        sessionId
                );
        }

        private void requirePhase(
                AccessRequest request,
                String expectedPhase,
                String operationName
        ) {
                if (!expectedPhase.equals(request.phase())) {
                        throw new IllegalArgumentException(
                                operationName + " requires " + expectedPhase + " phase"
                        );
                }
        }


        private void logEvaluation(
                Long sessionId,
                String eventType,
                String phase,
                EvaluationResult result,
                String message
        ) {
                PolicyDecision decision = result.policyDecision();
                sessionManager.logEvent(
                        sessionId,
                        eventType,
                        phase,
                        decision.rawDecision(),
                        decision.reasonCode(),
                        evaluationDetailsJson(result, message)
                );
        }

        private String evaluationDetailsJson(EvaluationResult result, String message) {
                return evaluationDetailsJson(result, message, null);
        }

        private String evaluationDetailsJson(
                EvaluationResult result,
                String message,
                Long durationMs
        ) {
                EvaluationContext context = result.evaluationContext();
                PolicyMetadata policy = result.policyMetadata();
                AccessRequest request = context.accessRequest();

                StringBuilder json = new StringBuilder();
                json.append("{");
                json.append("\"message\":").append(jsonValue(message));

                if (durationMs != null) {
                        json.append(",\"durationMs\":").append(durationMs);
                }

                json.append(",\"policy\":{");
                json.append("\"id\":").append(policy.policyId());
                json.append(",\"ref\":").append(jsonValue(policy.policyRef()));
                json.append(",\"name\":").append(jsonValue(policy.policyName()));
                json.append(",\"phase\":").append(jsonValue(policy.phase()));
                json.append(",\"accessPath\":").append(jsonValue(policy.accessPath()));
                json.append("}");

                json.append(",\"request\":{");
                json.append("\"subjectUserId\":").append(request.subjectUserId());
                json.append(",\"resourceType\":").append(jsonValue(request.resourceType()));
                json.append(",\"resourceId\":").append(request.resourceId());
                json.append(",\"actionId\":").append(jsonValue(request.actionId()));
                json.append(",\"accessPath\":").append(jsonValue(request.accessPath()));
                json.append(",\"phase\":").append(jsonValue(request.phase()));
                json.append("}");

                json.append(",\"checkedAttributes\":[");
                for (int i = 0; i < context.attributes().size(); i++) {
                        ResolvedAttribute attr = context.attributes().get(i);
                        if (i > 0) {
                                json.append(",");
                        }
                        json.append("{");
                        json.append("\"name\":").append(jsonValue(attr.attributeName()));
                        json.append(",\"xacmlId\":").append(jsonValue(attr.xacmlAttributeId()));
                        json.append(",\"category\":").append(jsonValue(attr.xacmlCategory()));
                        json.append(",\"dataType\":").append(jsonValue(attr.xacmlDataType()));
                        json.append(",\"value\":").append(jsonValue(attr.value()));
                        json.append("}");
                }
                json.append("]");

                json.append(",\"dependencies\":[");
                for (int i = 0; i < context.dependencies().size(); i++) {
                        SessionDependency dep = context.dependencies().get(i);
                        if (i > 0) {
                                json.append(",");
                        }
                        json.append("{");
                        json.append("\"key\":").append(jsonValue(dep.dependencyKey()));
                        json.append(",\"attrKey\":").append(jsonValue(dep.attrKey()));
                        json.append(",\"entityType\":").append(jsonValue(dep.entityType()));
                        json.append(",\"entityId\":").append(dep.entityId());
                        json.append(",\"attributeName\":").append(jsonValue(dep.attributeName()));
                        json.append(",\"monitorType\":").append(jsonValue(dep.monitorType()));
                        json.append(",\"nextCheckAt\":").append(jsonValue(dep.nextCheckAt()));
                        json.append("}");
                }
                json.append("]");

                json.append("}");
                return json.toString();
        }

        private String jsonValue(Object value) {
                if (value == null) {
                        return "null";
                }
                if (value instanceof Number || value instanceof Boolean) {
                        return String.valueOf(value);
                }
                return "\"" + escapeJson(String.valueOf(value)) + "\"";
        }

        private void log(
                Long sessionId,
                String eventType,
                String phase,
                PolicyDecision decision,
                String message
        ) {
                sessionManager.logEvent(
                        sessionId,
                        eventType,
                        phase,
                        decision.rawDecision(),
                        decision.reasonCode(),
                        jsonMessage(message)
                );
        }

        private String jsonMessage(String message) {
                return "{\"message\":\"" + escapeJson(message) + "\"}";
        }

        private String escapeJson(String value) {
                return value
                        .replace("\\", "\\\\")
                        .replace("\"", "\\\"");
        }

        private Long asLong(Map<String, Object> source, String key) {
                return ((Number) source.get(key)).longValue();
        }

        private String asString(Map<String, Object> source, String key) {
                return (String) source.get(key);
        }
}