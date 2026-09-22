package vn.ucon.demo.ucon.ch;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.springframework.context.ApplicationEventPublisher;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import vn.ucon.demo.ucon.model.AccessDecision;
import vn.ucon.demo.ucon.model.AccessEvaluationAttempt;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.AttributeUpdateInstruction;
import vn.ucon.demo.ucon.model.EvaluationContext;
import vn.ucon.demo.ucon.model.EvaluationResult;
import vn.ucon.demo.ucon.model.MonitorType;
import vn.ucon.demo.ucon.model.PolicyAttributeRequirement;
import vn.ucon.demo.ucon.model.PolicyDecision;
import vn.ucon.demo.ucon.model.PolicyMetadata;
import vn.ucon.demo.ucon.model.RequiredObligation;
import vn.ucon.demo.ucon.model.ResolvedAttribute;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.model.TriggerEvent;
import vn.ucon.demo.ucon.model.UsageGrant;
import vn.ucon.demo.ucon.model.UsageSessionEvent;
import vn.ucon.demo.ucon.obligation.ObligationManager;
import vn.ucon.demo.ucon.pap.PapService;
import vn.ucon.demo.ucon.pdp.AuthzForcePdpAdapter;
import vn.ucon.demo.ucon.pip.PipService;
import vn.ucon.demo.ucon.pip.provider.AttributeUpdateRequest;
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

        private static final String REASON_NO_AUTHORIZATION_GRANTED = "NO_AUTHORIZATION_GRANTED";
        private static final String REASON_ENTITY_DELETED = "ENTITY_DELETED";

        private final PapService papService;
        private final PipService pipService;
        private final AuthzForcePdpAdapter pdpAdapter;
        private final ObligationManager obligationManager;
        private final UsageSessionManager sessionManager;
        private final ApplicationEventPublisher eventPublisher;
        private final JdbcTemplate jdbcTemplate;

        public ContextHandler(
                PapService papService,
                PipService pipService,
                AuthzForcePdpAdapter pdpAdapter,
                ObligationManager obligationManager,
                UsageSessionManager sessionManager,
                ApplicationEventPublisher eventPublisher,
                JdbcTemplate jdbcTemplate
        ) {
                this.papService = papService;
                this.pipService = pipService;
                this.pdpAdapter = pdpAdapter;
                this.obligationManager = obligationManager;
                this.sessionManager = sessionManager;
                this.eventPublisher = eventPublisher;
                this.jdbcTemplate = jdbcTemplate;
        }

        /**
         * PRE phase entry point.
         *
         * If the request contains an explicit accessPath, CH evaluates only that PRE policy.
         * If accessPath is blank/AUTO, CH asks PAP for candidate PRE policies and tries them by priority.
         *
         * After a candidate policy is permitted, the usage session is bound to that policy/accessPath.
         * ONGOING and POST phases do not run candidate resolution again.
         */
        @Transactional
        public AccessDecision handleTryAccess(AccessRequest request) {
                requirePhase(request, PHASE_PRE, "handleTryAccess");
                return evaluatePreCandidates(request, candidatePrePolicies(request));
        }

        @Transactional
        public AccessDecision handleStartAccess(Long sessionId) {
                if (sessionManager.isActive(sessionId)) {
                        return decision(
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

                if (!decision.permit()) {
                        LocalDateTime deniedAt = sessionManager.denyPendingSession(
                                sessionId,
                                decision.reasonCode(),
                                startResult.evaluatedAt()
                        );

                        logEventAt(
                                sessionId,
                                "DENY_START_ACCESS",
                                PHASE_ONGOING,
                                decision.rawDecision(),
                                decision.reasonCode(),
                                null,
                                deniedAt,
                                evaluationDetailsJson(
                                        startResult,
                                        "Pending session activation denied before actual resource usage"
                                )
                        );

                        LocalDateTime postProcessedAt =
                                sessionManager.completePostProcessing(sessionId);

                        logPostProcessingCompleted(
                                sessionId,
                                "DENY_START:" + decision.reasonCode(),
                                postProcessedAt
                        );

                        return toDecision(decision, sessionId);
                }

                LocalDateTime activatedAt =
                        sessionManager.activateSession(
                                sessionId,
                                startResult.evaluatedAt()
                        );

                logEventAt(
                        sessionId,
                        "START_ACCESS",
                        PHASE_ONGOING,
                        decision.rawDecision(),
                        decision.reasonCode(),
                        null,
                        activatedAt,
                        evaluationDetailsJson(
                                startResult,
                                "Session activated after ONGOING policy check permitted start access"
                        )
                );

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

                affectedSessionIds.forEach(
                        sessionId -> reevaluate(sessionId, triggerEvent)
                );
        }

        @Transactional
        public AccessDecision handleEndAccess(Long sessionId) {
                if (!sessionManager.isActive(sessionId)) {
                        return deny("SESSION_NOT_ACTIVE", sessionId);
                }

                LocalDateTime endedAt =
                        sessionManager.endSession(sessionId);

                logEventAt(
                        sessionId,
                        "END_ACCESS",
                        PHASE_POST,
                        RAW_PERMIT,
                        "USER_END",
                        null,
                        endedAt,
                        jsonMessage("User ended the usage session")
                );

                processPostAttributeUpdates(sessionId, "USER_END", null);

                eventPublisher.publishEvent(
                        UsageSessionEvent.ended(sessionId, "USER_END")
                );

                return decision(
                        true,
                        RAW_PERMIT,
                        "USER_END",
                        sessionId
                );
        }

        private List<PolicyMetadata> candidatePrePolicies(AccessRequest request) {
                if (hasExplicitAccessPath(request.accessPath())) {
                        AccessRequest normalizedRequest = withAccessPath(
                                request,
                                normalizeAccessPath(request.accessPath()),
                                PHASE_PRE,
                                null
                        );

                        return List.of(findPolicy(normalizedRequest, PHASE_PRE));
                }

                return papService.findCandidatePolicies(
                        request.resourceType(),
                        request.actionId(),
                        PHASE_PRE
                );
        }

        private AccessDecision evaluatePreCandidates(
                AccessRequest baseRequest,
                List<PolicyMetadata> candidatePolicies
        ) {
                List<AccessEvaluationAttempt> attempts = new ArrayList<>();

                for (PolicyMetadata candidatePolicy : candidatePolicies) {
                        AccessRequest candidateRequest = withAccessPath(
                                baseRequest,
                                candidatePolicy.accessPath(),
                                PHASE_PRE,
                                null
                        );

                        EvaluationResult preResult;

                        try {
                                preResult = evaluateWithPolicy(
                                        candidateRequest,
                                        candidatePolicy,
                                        List.of()
                                );
                        } catch (IllegalStateException ex) {
                                if (isMissingCandidateAttribute(ex)) {
                                        // Candidate này không có đủ context để áp dụng.
                                        // Ví dụ user không có user_rentals khi đang thử RENTAL.
                                         attempts.add(AccessEvaluationAttempt.skipped(
                                                candidatePolicy.accessPath(),
                                                candidatePolicy,
                                                "MISSING_REQUIRED_ATTRIBUTE",
                                                ex.getMessage()
                                        ));
                                        continue;
                                }
                                // Các lỗi lập trình hoặc cấu hình khác vẫn phải được báo ra.
                                throw ex;
                        }

                        PolicyDecision preDecision = preResult.policyDecision();

                        List<RequiredObligation> requiredObligations =
                                obligationManager.resolveRequiredObligations(
                                        preDecision.requiredObligations()
                                );

                        attempts.add(AccessEvaluationAttempt.evaluated(
                                candidatePolicy.accessPath(),
                                candidatePolicy,
                                preDecision,
                                requiredObligations
                        ));

                        if (!preDecision.permit()) {
                                continue;
                        }

                        PolicyMetadata ongoingPolicy = findPolicy(
                                candidateRequest,
                                PHASE_ONGOING
                        );
                        PolicyMetadata postPolicy = findPolicy(
                                candidateRequest,
                                PHASE_POST
                        );

                        List<SessionDependency> dependencies = resolveDependencies(
                                candidateRequest,
                                ongoingPolicy
                        );

                        Map.Entry<Long, LocalDateTime> createdSession =
                                sessionManager.createPendingSession(
                                        candidateRequest,
                                        List.of(
                                                preResult.policyMetadata(),
                                                ongoingPolicy,
                                                postPolicy
                                        ),
                                        dependencies,
                                        preResult.evaluatedAt()
                                );

                        Long sessionId = createdSession.getKey();
                        LocalDateTime createdAt = createdSession.getValue();

                        logEventAt(
                                sessionId,
                                "PRE_ACCESS_GRANTED",
                                PHASE_PRE,
                                preDecision.rawDecision(),
                                preDecision.reasonCode(),
                                null,
                                createdAt,
                                candidatePermitDetailsJson(
                                        baseRequest,
                                        candidatePolicies,
                                        attempts,
                                        preResult,
                                        sessionId
                                )
                        );

                        return new AccessDecision(
                                true,
                                preDecision.rawDecision(),
                                preDecision.reasonCode(),
                                sessionId,
                                candidatePolicy.accessPath(),
                                attempts
                        );
                }

                String finalReasonCode = resolvePreDenyReason(attempts);

                logEvent(
                        null,
                        "DENY_ACCESS",
                        PHASE_PRE,
                        RAW_DENY,
                        finalReasonCode,
                        null,
                        candidateDenyDetailsJson(
                                baseRequest,
                                candidatePolicies,
                                attempts,
                                finalReasonCode
                        )
                );

                return new AccessDecision(
                        false,
                        RAW_DENY,
                        finalReasonCode,
                        null,
                        null,
                        attempts
                );
        }

        private String resolvePreDenyReason(
                List<AccessEvaluationAttempt> attempts
        ) {
                return attempts.stream()
                        .filter(attempt ->
                                attempt.requiredObligations() != null
                                        && !attempt.requiredObligations().isEmpty()
                        )
                        .map(AccessEvaluationAttempt::reasonCode)
                        .filter(reasonCode ->
                                reasonCode != null
                                        && !reasonCode.isBlank()
                        )
                        .findFirst()
                        .orElse(REASON_NO_AUTHORIZATION_GRANTED);
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
                Long triggerEventId = triggerEvent.entityId();

                logEvent(
                        sessionId,
                        "ENTITY_DELETED_TRIGGER",
                        PHASE_ONGOING,
                        RAW_DENY,
                        reasonCode,
                        triggerEventId,
                        jsonMessage(
                                "CH received ENTITY_DELETED trigger for "
                                        + triggerEvent.entityType()
                                        + "#"
                                        + triggerEventId
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

        private void reevaluate(
                Long sessionId,
                TriggerEvent triggerEvent
        ) {
                if (!sessionManager.isActive(sessionId)) {
                        return;
                }

                PolicyDecision decision = evalOngoing(
                        sessionId,
                        "EVENT_REEVALUATE",
                        "CH re-evaluated ongoing policy after trigger event",
                        triggerEvent.id()
                );

                if (!decision.permit()) {
                        revoke(
                                sessionId,
                                decision.reasonCode(),
                                triggerEvent
                        );
                }
        }

        private PolicyDecision evalOngoing(
                Long sessionId,
                String eventType,
                String message,
                Long triggerEventId
        ) {
                long startedAt = System.nanoTime();

                EvaluationResult ongoingResult = evaluateSession(
                        sessionId,
                        PHASE_ONGOING
                );

                long elapsedNs = System.nanoTime() - startedAt;
                double durationMs = elapsedNs / 1_000_000.0;

                PolicyDecision decision = ongoingResult.policyDecision();

                logEvaluationAtAndTouchLastEval(
                        sessionId,
                        eventType,
                        PHASE_ONGOING,
                        decision.rawDecision(),
                        decision.reasonCode(),
                        triggerEventId,
                        ongoingResult.evaluatedAt(),
                        evaluationDetailsJson(
                                ongoingResult,
                                message,
                                durationMs
                        )
                );

                return decision;
        }

        private EvaluationResult evaluateSession(
                Long sessionId,
                String phase
        ) {
                Map<String, Object> session =
                        sessionManager.findSessionWithBoundPolicies(sessionId);

                AccessRequest request =
                        requestFromSession(
                                session,
                                sessionId,
                                phase
                        );

                List<SessionDependency> dependencies =
                        sessionManager.findDependencies(sessionId);

                String policyRef =
                        boundPolicyRef(session, phase);

                PolicyMetadata policyMetadata =
                        papService.findPolicyMetadata(policyRef);

                return evaluateWithPolicy(
                        request,
                        policyMetadata,
                        dependencies
                );
        }

        private EvaluationResult evaluateWithPolicy(
                AccessRequest request,
                PolicyMetadata policyMetadata,
                List<SessionDependency> dependencies
        ) {
                List<ResolvedAttribute> attributes = pipService.resolveAttributes(
                        request,
                        policyMetadata.policyRef(),
                        dependencies,
                        policyMetadata.evaluationAttributes()
                );

                validateRequiredAttributes(
                        policyMetadata.evaluationAttributes(),
                        attributes
                );

                EvaluationContext context = buildContext(
                        request,
                        policyMetadata,
                        dependencies,
                        attributes
                );

                PolicyDecision decision = pdpAdapter.evaluate(context);

                LocalDateTime evaluatedAt = jdbcTemplate.queryForObject(
                        "SELECT clock_timestamp()::timestamp",
                        LocalDateTime.class
                );

                return new EvaluationResult(
                        policyMetadata,
                        context,
                        decision,
                        evaluatedAt
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
                AccessRequest ongoingRequest = withAccessPath(
                        preRequest,
                        preRequest.accessPath(),
                        PHASE_ONGOING,
                        null
                );

                return pipService.resolveDependencies(
                        ongoingRequest,
                        ongoingPolicy.ongoingDependencyAttributes()
                );
        }

        private AccessRequest requestFromSession(
                Map<String, Object> session,
                Long sessionId,
                String phase
        ) {
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

        private String boundPolicyRef(
                Map<String, Object> session,
                String phase
        ) {
                String columnName = switch (phase) {
                        case PHASE_PRE ->
                                "pre_policy_ref";

                        case PHASE_ONGOING ->
                                "ongoing_policy_ref";

                        case PHASE_POST ->
                                "post_policy_ref";

                        default -> throw new IllegalArgumentException(
                                "Unsupported policy phase: " + phase
                        );
                };

                String policyRef =
                        asString(session, columnName);

                if (policyRef == null || policyRef.isBlank()) {
                        throw new IllegalStateException(
                                "No bound " + phase
                                        + " policy found for sessionId="
                                        + asLong(session, "id")
                        );
                }

                return policyRef;
        }

        private AccessRequest withAccessPath(
                AccessRequest source,
                String accessPath,
                String phase,
                Long usageSessionId
        ) {
                return new AccessRequest(
                        source.subjectUserId(),
                        source.resourceType(),
                        source.resourceId(),
                        source.actionId(),
                        accessPath,
                        phase,
                        usageSessionId
                );
        }

        private void revoke(
                Long sessionId,
                String reasonCode,
                TriggerEvent triggerEvent
        ) {
                if (!sessionManager.isActive(sessionId)) {
                        return;
                }

                LocalDateTime revokedAt =
                        sessionManager.revokeSession(sessionId, reasonCode);

                logEventAt(
                        sessionId,
                        "REVOKE_ACCESS",
                        PHASE_ONGOING,
                        RAW_DENY,
                        reasonCode,
                        null,
                        revokedAt,
                        jsonMessage(
                                "Session revoked after ONGOING policy became unsatisfied"
                        )
                );

                processPostAttributeUpdates(
                        sessionId,
                        "REVOKE:" + reasonCode,
                        triggerEvent
                );
                
                publishRevoked(sessionId, reasonCode);
        }

        private void publishRevoked(Long sessionId, String reasonCode) {
                eventPublisher.publishEvent(
                        UsageSessionEvent.revoked(sessionId, reasonCode)
                );

                logEvent(
                        sessionId,
                        "PEP_REVOKE_SIGNAL_SENT",
                        PHASE_ONGOING,
                        RAW_DENY,
                        reasonCode,
                        null,
                        jsonMessage("CH published revoke signal for PEP")
                );
        }

        private void processPostAttributeUpdates(
                Long sessionId,
                String cause,
                TriggerEvent triggerEvent
        ) {
                if (sessionManager.isPostProcessingCompleted(sessionId)) {
                        logEvent(
                                sessionId,
                                "POST_PROCESSING_SKIPPED",
                                PHASE_POST,
                                RAW_PERMIT,
                                cause,
                                null,
                                jsonMessage("Post processing already completed for this session")
                        );

                        return;
                }

                EvaluationResult postResult = evaluateSession(sessionId, PHASE_POST);
                PolicyDecision postDecision = postResult.policyDecision();

                logEvent(
                        sessionId,
                        "POST_POLICY_EVALUATED",
                        PHASE_POST,
                        postDecision.rawDecision(),
                        cause,
                        null,
                        evaluationDetailsJson(
                                postResult,
                                "CH evaluated the POST policy and collected attribute-update instructions"
                        )
                );

                if (!postDecision.permit()) {
                        throw new IllegalStateException(
                                "POST policy was not permitted for sessionId="
                                        + sessionId
                                        + ", decision="
                                        + postDecision.rawDecision()
                        );
                }

                applyAttributeUpdates(
                        sessionId,
                        postDecision.attributeUpdateInstructions(),
                        triggerEvent
                );
                LocalDateTime postProcessedAt =
                        sessionManager.completePostProcessing(sessionId);

                logPostProcessingCompleted(
                        sessionId,
                        cause,
                        postProcessedAt
                );
        }

        private void applyAttributeUpdates(
                Long sessionId,
                List<AttributeUpdateInstruction> updates,
                TriggerEvent triggerEvent
        ) {
                updates.forEach(update -> applyAttributeUpdate(
                        sessionId,
                        update,
                        triggerEvent
                ));
        }

        private void applyAttributeUpdate(
                Long sessionId,
                AttributeUpdateInstruction update,
                TriggerEvent triggerEvent
        ) {
                boolean applied = pipService.applyAttributeUpdate(
                        new AttributeUpdateRequest(
                                update.actionCode(),
                                sessionId,
                                null
                        ),
                        triggerEvent
                );

                if (!applied) {
                        logEvent(
                                sessionId,
                                "ATTRIBUTE_UPDATE_SKIPPED",
                                PHASE_POST,
                                RAW_DENY,
                                update.actionCode(),
                                null,
                                jsonMessage("PIP skipped update action because target entity was deleted")
                        );
                        return;
                }

                logEvent(
                        sessionId,
                        "ATTRIBUTE_UPDATE",
                        PHASE_POST,
                        RAW_PERMIT,
                        update.actionCode(),
                        null,
                        jsonMessage("CH instructed the PIP to apply the attribute update")
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

        private AccessDecision toDecision(
                PolicyDecision policyDecision,
                Long sessionId
        ) {
                // return new AccessDecision(
                //         policyDecision.permit(),
                //         policyDecision.rawDecision(),
                //         policyDecision.reasonCode(),
                //         sessionId,
                //         null,
                //         List.of()
                return decision(
                        policyDecision.permit(),
                        policyDecision.rawDecision(),
                        policyDecision.reasonCode(),
                        sessionId
                );
        }

        private AccessDecision deny(String reasonCode, Long sessionId) {
                return decision(
                        false,
                        RAW_DENY,
                        reasonCode,
                        sessionId
                );
        }

        private AccessDecision decision(
                boolean permit,
                String rawDecision,
                String reasonCode,
                Long sessionId
        ) {
                return new AccessDecision(
                        permit,
                        rawDecision,
                        reasonCode,
                        sessionId,
                        null,
                        List.of()
                );
        }

        private boolean hasExplicitAccessPath(String accessPath) {
                return accessPath != null
                        && !accessPath.isBlank()
                        && !"AUTO".equalsIgnoreCase(accessPath);
        }

        private String normalizeAccessPath(String value) {
                return value.trim()
                        .toUpperCase(Locale.ROOT)
                        .replace("-", "_");
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

        private void validateRequiredAttributes(
                List<PolicyAttributeRequirement> requirements,
                List<ResolvedAttribute> attributes
        ) {
                for (PolicyAttributeRequirement requirement : requirements) {
                        if (!requirement.required()) {
                                continue;
                        }

                        boolean present = attributes.stream()
                                .anyMatch(attribute ->
                                        requirement.attrKey().equals(
                                                attribute.attributeName()
                                        )
                                        && attribute.value() != null
                                );

                        if (!present) {
                                throw new IllegalStateException(
                                        "Missing required attribute: "
                                                + requirement.attrKey()
                                );
                        }
                }
        }

        private boolean isMissingCandidateAttribute(IllegalStateException ex) {
                String message = ex.getMessage();

                return message != null
                        && message.startsWith("Missing required attribute:");
        }

        private void logEvent(
                Long sessionId,
                String eventType,
                String phase,
                String decision,
                String reasonCode,
                Long triggerEventId,
                String detailsJson
        ) {
                jdbcTemplate.update("""
                        INSERT INTO session_events(
                                usage_session_id,
                                trigger_event_id,
                                event_type,
                                phase,
                                decision,
                                reason_code,
                                event_at,
                                logged_at,
                                details_json
                        )
                        VALUES (
                                ?, ?,
                                ?, ?, ?, ?,
                                clock_timestamp(),
                                clock_timestamp(),
                                ?::jsonb
                        )
                        """,
                        sessionId,
                        triggerEventId,
                        eventType,
                        phase,
                        decision,
                        reasonCode,
                        detailsJson
                );
        }

        private void logEvaluationAtAndTouchLastEval(
                Long sessionId,
                String eventType,
                String phase,
                String decision,
                String reasonCode,
                Long triggerEventId,
                LocalDateTime evaluatedAt,
                String detailsJson
        ) {
                if (sessionId == null) {
                        logEventAt(
                                null,
                                eventType,
                                phase,
                                decision,
                                reasonCode,
                                triggerEventId,
                                evaluatedAt,
                                detailsJson
                        );
                        return;
                }

                jdbcTemplate.update("""
                        WITH inserted_event AS (
                                INSERT INTO session_events(
                                        usage_session_id,
                                        trigger_event_id,
                                        event_type,
                                        phase,
                                        decision,
                                        reason_code,
                                        event_at,
                                        logged_at,
                                        details_json
                                )
                                VALUES (
                                        ?, ?,
                                        ?, ?, ?, ?,
                                        ?,
                                        clock_timestamp(),
                                        ?::jsonb
                                )
                                RETURNING event_at
                        )
                        UPDATE usage_sessions us
                        SET last_eval_at = inserted_event.event_at
                        FROM inserted_event
                        WHERE us.id = ?
                        """,
                        sessionId,
                        triggerEventId,
                        eventType,
                        phase,
                        decision,
                        reasonCode,
                        evaluatedAt,
                        detailsJson,
                        sessionId
                );
        }

        private void logEventAt(
                Long sessionId,
                String eventType,
                String phase,
                String decision,
                String reasonCode,
                Long triggerEventId,
                LocalDateTime eventAt,
                String detailsJson
        ) {
                jdbcTemplate.update("""
                        INSERT INTO session_events(
                                usage_session_id,
                                trigger_event_id,
                                event_type,
                                phase,
                                decision,
                                reason_code,
                                event_at,
                                logged_at,
                                details_json
                        )
                        VALUES (
                                ?, ?,
                                ?, ?, ?, ?,
                                ?,
                                clock_timestamp(),
                                ?::jsonb
                        )
                        """,
                        sessionId,
                        triggerEventId,
                        eventType,
                        phase,
                        decision,
                        reasonCode,
                        eventAt,
                        detailsJson
                );
        }

        private void logPostProcessingCompleted(
                Long sessionId,
                String cause,
                LocalDateTime eventAt
        ) {
                logEventAt(
                        sessionId,
                        "POST_PROCESSING_COMPLETED",
                        PHASE_POST,
                        RAW_PERMIT,
                        "POST_PROCESSING_COMPLETED",
                        null,
                        eventAt,
                        jsonMessage(
                                "Post processing completed for cause: " + cause
                        )
                );
        }

        private String evaluationDetailsJson(EvaluationResult result, String message) {
                return evaluationDetailsJson(result, message, null);
        }

        private String evaluationDetailsJson(
                EvaluationResult result,
                String message,
                Double durationMs
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

        private String candidatePermitDetailsJson(
                AccessRequest baseRequest,
                List<PolicyMetadata> candidatePolicies,
                List<AccessEvaluationAttempt> attempts,
                EvaluationResult selectedResult,
                Long sessionId
        ) {
                EvaluationContext context = selectedResult.evaluationContext();
                PolicyMetadata policy = selectedResult.policyMetadata();
                AccessRequest selectedRequest = context.accessRequest();

                StringBuilder json = new StringBuilder();
                json.append("{");

                // Giữ cùng cấu trúc chính với evaluationDetailsJson(...)
                // để PRE_ACCESS_GRANTED, START_ACCESS và EVENT_REEVALUATE có format nhất quán.
                json.append("\"message\":").append(jsonValue(
                        "CH evaluated PRE candidate policy, selected an authorization path, and created pending session"
                ));

                json.append(",\"policy\":{");
                json.append("\"id\":").append(policy.policyId());
                json.append(",\"ref\":").append(jsonValue(policy.policyRef()));
                json.append(",\"name\":").append(jsonValue(policy.policyName()));
                json.append(",\"phase\":").append(jsonValue(policy.phase()));
                json.append(",\"accessPath\":").append(jsonValue(policy.accessPath()));
                json.append("}");

                json.append(",\"request\":{");
                json.append("\"subjectUserId\":").append(selectedRequest.subjectUserId());
                json.append(",\"resourceType\":").append(jsonValue(selectedRequest.resourceType()));
                json.append(",\"resourceId\":").append(selectedRequest.resourceId());
                json.append(",\"actionId\":").append(jsonValue(selectedRequest.actionId()));
                json.append(",\"accessPath\":").append(jsonValue(selectedRequest.accessPath()));
                json.append(",\"phase\":").append(jsonValue(selectedRequest.phase()));
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

                // Thông tin bổ sung riêng cho PRE candidate flow.
                // Các field này nằm sau phần evaluation chuẩn, không thay thế checkedAttributes/dependencies.
                json.append(",\"session\":{");
                json.append("\"id\":").append(sessionId);
                json.append(",\"status\":").append(jsonValue("PENDING"));
                json.append("}");

                json.append(",\"candidateRequest\":{");
                json.append("\"requestedAccessPath\":").append(jsonValue(baseRequest.accessPath()));
                json.append("}");

                json.append(",\"candidatePolicies\":[");
                for (int i = 0; i < candidatePolicies.size(); i++) {
                        PolicyMetadata candidate = candidatePolicies.get(i);
                        if (i > 0) {
                                json.append(",");
                        }

                        json.append("{");
                        json.append("\"id\":").append(candidate.policyId());
                        json.append(",\"ref\":").append(jsonValue(candidate.policyRef()));
                        json.append(",\"name\":").append(jsonValue(candidate.policyName()));
                        json.append(",\"accessPath\":").append(jsonValue(candidate.accessPath()));
                        json.append("}");
                }
                json.append("]");

                json.append(",\"attempts\":[");
                for (int i = 0; i < attempts.size(); i++) {
                        AccessEvaluationAttempt attempt = attempts.get(i);
                        if (i > 0) {
                                json.append(",");
                        }

                        json.append("{");
                        json.append("\"accessPath\":").append(jsonValue(attempt.accessPath()));
                        json.append(",\"result\":").append(jsonValue(attempt.result()));
                        json.append(",\"reasonCode\":").append(jsonValue(attempt.reasonCode()));

                        if (attempt.rawDecision() != null) {
                                json.append(",\"rawDecision\":").append(jsonValue(attempt.rawDecision()));
                        }

                        if (attempt.policyRef() != null) {
                                json.append(",\"policyRef\":").append(jsonValue(attempt.policyRef()));
                        }

                        if (attempt.message() != null) {
                                json.append(",\"message\":").append(jsonValue(attempt.message()));
                        }

                        json.append("}");
                }
                json.append("]");

                json.append("}");
                return json.toString();
        }

        private String candidateDenyDetailsJson(
                AccessRequest request,
                List<PolicyMetadata> candidatePolicies,
                List<AccessEvaluationAttempt> attempts,
                String finalReasonCode
        ) {
                StringBuilder json = new StringBuilder();

                json.append("{");
                json.append("\"message\":").append(jsonValue("No candidate policy granted this access request"));
                json.append(",\"reasonCode\":").append(jsonValue(finalReasonCode));

                json.append(",\"request\":{");
                json.append("\"subjectUserId\":").append(request.subjectUserId());
                json.append(",\"resourceType\":").append(jsonValue(request.resourceType()));
                json.append(",\"resourceId\":").append(request.resourceId());
                json.append(",\"actionId\":").append(jsonValue(request.actionId()));
                json.append(",\"requestedAccessPath\":").append(jsonValue(request.accessPath()));
                json.append(",\"phase\":").append(jsonValue(request.phase()));
                json.append("}");

                json.append(",\"candidatePolicies\":[");
                for (int i = 0; i < candidatePolicies.size(); i++) {
                        PolicyMetadata policy = candidatePolicies.get(i);
                        if (i > 0) {
                                json.append(",");
                        }

                        json.append("{");
                        json.append("\"id\":").append(policy.policyId());
                        json.append(",\"ref\":").append(jsonValue(policy.policyRef()));
                        json.append(",\"name\":").append(jsonValue(policy.policyName()));
                        json.append(",\"accessPath\":").append(jsonValue(policy.accessPath()));
                        json.append("}");
                }
                json.append("]");

                json.append(",\"attempts\":[");
                for (int i = 0; i < attempts.size(); i++) {
                        AccessEvaluationAttempt attempt = attempts.get(i);
                        if (i > 0) {
                                json.append(",");
                        }

                        json.append("{");
                        json.append("\"accessPath\":").append(jsonValue(attempt.accessPath()));
                        json.append(",\"result\":").append(jsonValue(attempt.result()));
                        json.append(",\"reasonCode\":").append(jsonValue(attempt.reasonCode()));

                        if (attempt.rawDecision() != null) {
                                json.append(",\"rawDecision\":").append(jsonValue(attempt.rawDecision()));
                        }

                        if (attempt.policyRef() != null) {
                                json.append(",\"policyRef\":").append(jsonValue(attempt.policyRef()));
                        }

                        if (attempt.message() != null) {
                                json.append(",\"message\":").append(jsonValue(attempt.message()));
                        }

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

        private String jsonMessage(String message) {
                return "{\"message\":" + jsonValue(message) + "}";
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