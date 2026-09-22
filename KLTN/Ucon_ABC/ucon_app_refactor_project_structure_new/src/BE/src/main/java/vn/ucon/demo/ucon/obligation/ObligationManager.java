package vn.ucon.demo.ucon.obligation;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import vn.ucon.demo.ucon.model.RequiredObligation;

import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.server.ResponseStatusException;

import java.util.List;
import java.util.Map;

@Service
public class ObligationManager {

        private static final String SCOPE_USER_GLOBAL =
                "USER_GLOBAL";

        private static final String SCOPE_USER_POLICY =
                "USER_POLICY";

        private final JdbcTemplate jdbcTemplate;
        private final ObjectMapper objectMapper;
        private final List<ObligationFulfillmentHandler> fulfillmentHandlers;

        public ObligationManager(
                JdbcTemplate jdbcTemplate,
                ObjectMapper objectMapper,
                List<ObligationFulfillmentHandler> fulfillmentHandlers
        ) {
                this.jdbcTemplate = jdbcTemplate;
                this.objectMapper = objectMapper;
                this.fulfillmentHandlers = fulfillmentHandlers;
        }

        public List<RequiredObligation> resolveRequiredObligations(
                List<RequiredObligation> rawObligations
        ) {
                if (rawObligations == null || rawObligations.isEmpty()) {
                        return List.of();
                }

                return rawObligations.stream()
                        .map(this::resolveRequiredObligation)
                        .toList();
        }

        private RequiredObligation resolveRequiredObligation(
                RequiredObligation raw
        ) {
                ObligationDefinition definition = findDefinition(raw.obligationId());

                /*
                * Advice và catalog phải cùng phiên bản.
                * Nếu không khớp thì đây là lỗi cấu hình policy.
                */
                if (raw.version() != null
                        && !raw.version().equals(definition.version())) {
                        throw new IllegalStateException(
                                "Obligation version mismatch for "
                                        + raw.obligationId()
                                        + ": policy="
                                        + raw.version()
                                        + ", catalog="
                                        + definition.version()
                        );
                }

                return new RequiredObligation(
                        raw.obligationId(),
                        definition.type(),
                        definition.version(),
                        raw.policyRef(),
                        definition.title(),
                        definition.description()
                );
        }

        @Transactional
        public FulfillObligationResult fulfill(
                String obligationId,
                FulfillObligationRequest request
        ) {
                validateRequest(request);

                ObligationDefinition definition = findDefinition(obligationId);

                /*
                * policyRef đến từ attempt đã trả về cho PEP/frontend.
                *
                * Kiểm tra này ngăn client tùy ý ghi evidence cho một policy
                * không hề yêu cầu obligation tương ứng.
                */
                requirePolicyUsesObligation(
                        request.policyRef(),
                        obligationId
                );

                Map<String, Object> evidence =
                        request.evidence() == null
                                ? Map.of()
                                : request.evidence();

                /*
                * Mỗi obligation type có handler riêng.
                *
                * Ví dụ:
                * CONSENT -> ConsentObligationHandler
                * MFA     -> MfaObligationHandler (nếu bổ sung sau này)
                */
                ObligationFulfillmentHandler handler = findHandler(definition.type());

                try {
                        handler.validate(evidence);
                } catch (IllegalArgumentException ex) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                ex.getMessage(),
                                ex
                        );
                }

                String evidenceJson = toJson(evidence);

                /*
                * Scope chỉ quyết định cách xác định khóa lưu evidence.
                *
                * USER_GLOBAL:
                * obligation + version + user
                *
                * USER_POLICY:
                * obligation + version + user + policy
                */
                switch (definition.scope()) {
                case SCOPE_USER_GLOBAL ->
                        saveUserGlobalEvidence(
                                definition,
                                request.userId(),
                                evidenceJson
                        );

                case SCOPE_USER_POLICY ->
                        saveUserPolicyEvidence(
                                definition,
                                request.userId(),
                                request.policyRef(),
                                evidenceJson
                        );

                default -> throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "Unsupported obligation scope: "
                                        + definition.scope()
                        );
                }

                return new FulfillObligationResult(
                        "OBLIGATION_FULFILLED",
                        definition.obligationId(),
                        definition.version(),
                        definition.type(),
                        definition.scope(),
                        request.userId(),
                        request.policyRef()
                );
        }

        private void validateRequest(
                FulfillObligationRequest request
        ) {
                if (request == null) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "Request body is required"
                        );
                }

                if (request.userId() == null) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "userId is required"
                        );
                }

                /*
                * Kể cả USER_GLOBAL vẫn yêu cầu policyRef trong request
                * để xác minh obligation thực sự đến từ một policy/attempt hợp lệ.
                *
                * Với USER_GLOBAL, policyRef chỉ dùng để xác minh và không được
                * lưu thành một phần của khóa evidence.
                */
                if (request.policyRef() == null
                        || request.policyRef().isBlank()) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "policyRef is required"
                        );
                }
        }

        private ObligationDefinition findDefinition(
                        String obligationId
        ) {
                List<ObligationDefinition> rows =
                        jdbcTemplate.query(
                                """
                                SELECT obligation_id,
                                        version,
                                        obligation_type,
                                        scope,
                                        title,
                                        description
                                FROM obligation_catalog
                                WHERE obligation_id = ?
                                AND enabled = true
                                """,
                                (rs, rowNum) ->
                                        new ObligationDefinition(
                                                rs.getString(
                                                        "obligation_id"
                                                ),
                                                rs.getString(
                                                        "version"
                                                ),
                                                rs.getString(
                                                        "obligation_type"
                                                ),
                                                rs.getString(
                                                        "scope"
                                                ),
                                                rs.getString(
                                                        "title"
                                                ),
                                                rs.getString(
                                                        "description"
                                                )
                                        ),
                                obligationId
                        );

                if (rows.isEmpty()) {
                        throw new ResponseStatusException(
                                HttpStatus.NOT_FOUND,
                                "Unknown or disabled obligation: "
                                        + obligationId
                        );
                }

                return rows.get(0);
        }
                
        private void requirePolicyUsesObligation(
                String policyRef,
                String obligationId
        ) {
                String attrKeyPattern =
                        "obligation."
                                + obligationId
                                + ".%";

                Boolean exists =
                        jdbcTemplate.queryForObject(
                                """
                                SELECT EXISTS (
                                SELECT 1
                                FROM policy_registry pr
                                JOIN policy_attribute_requirements par
                                ON par.policy_id = pr.id
                                WHERE pr.policy_ref = ?
                                AND pr.phase = 'PRE'
                                AND pr.enabled = true
                                AND par.usage_type = 'EVALUATION'
                                AND par.attr_key LIKE ?
                                )
                                """,
                                Boolean.class,
                                policyRef,
                                attrKeyPattern
                        );

                if (!Boolean.TRUE.equals(exists)) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "Policy "
                                        + policyRef
                                        + " does not require obligation "
                                        + obligationId
                        );
                }
        }

        private ObligationFulfillmentHandler findHandler(
                String obligationType
        ) {
                return fulfillmentHandlers.stream()
                        .filter(handler ->
                                handler.supports(obligationType)
                        )
                        .findFirst()
                        .orElseThrow(() ->
                                new ResponseStatusException(
                                        HttpStatus.BAD_REQUEST,
                                        "Unsupported obligation type: "
                                                + obligationType
                                )
                        );
        }

        private void saveUserGlobalEvidence(
                ObligationDefinition definition,
                Long userId,
                String evidenceJson
        ) {
                jdbcTemplate.update(
                        """
                        INSERT INTO obligation_evidence(
                        obligation_id,
                        obligation_version,
                        subject_user_id,
                        policy_ref,
                        status,
                        fulfilled_at,
                        expires_at,
                        evidence_json
                        )
                        VALUES (
                        ?, ?, ?, NULL,
                        'FULFILLED',
                        now(),
                        NULL,
                        ?::jsonb
                        )

                        ON CONFLICT (
                        obligation_id,
                        obligation_version,
                        subject_user_id
                        )
                        WHERE policy_ref IS NULL

                        DO UPDATE SET
                        status = 'FULFILLED',
                        fulfilled_at = now(),
                        expires_at = NULL,
                        evidence_json =
                                EXCLUDED.evidence_json
                        """,
                        definition.obligationId(),
                        definition.version(),
                        userId,
                        evidenceJson
                );
        }

        private void saveUserPolicyEvidence(
                ObligationDefinition definition,
                Long userId,
                String policyRef,
                String evidenceJson
        ) {
                jdbcTemplate.update(
                        """
                        INSERT INTO obligation_evidence(
                        obligation_id,
                        obligation_version,
                        subject_user_id,
                        policy_ref,
                        status,
                        fulfilled_at,
                        expires_at,
                        evidence_json
                        )
                        VALUES (
                        ?, ?, ?, ?,
                        'FULFILLED',
                        now(),
                        NULL,
                        ?::jsonb
                        )

                        ON CONFLICT (
                        obligation_id,
                        obligation_version,
                        subject_user_id,
                        policy_ref
                        )
                        WHERE policy_ref IS NOT NULL

                        DO UPDATE SET
                        status = 'FULFILLED',
                        fulfilled_at = now(),
                        expires_at = NULL,
                        evidence_json =
                                EXCLUDED.evidence_json
                        """,
                        definition.obligationId(),
                        definition.version(),
                        userId,
                        policyRef,
                        evidenceJson
                );
        }

        private String toJson(
                Map<String, Object> evidence
        ) {
                try {
                        return objectMapper.writeValueAsString(
                                evidence == null
                                        ? Map.of()
                                        : evidence
                        );
                } catch (JsonProcessingException ex) {
                        throw new ResponseStatusException(
                                HttpStatus.BAD_REQUEST,
                                "Invalid evidence data",
                                ex
                        );
                }
        }

       private record ObligationDefinition(
                String obligationId,
                String version,
                String type,
                String scope,
                String title,
                String description
        ) {
        }
}