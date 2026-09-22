package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

import java.util.List;

@Component
public class ObligationEvidenceAttributeReader
        implements AttributeReader {

    private static final String READER_NAME =
            "ObligationEvidenceAttributeReader";

    private static final String ATTR_PREFIX =
            "obligation.";

    private final JdbcTemplate jdbcTemplate;

    public ObligationEvidenceAttributeReader(
            JdbcTemplate jdbcTemplate
    ) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return READER_NAME.equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        ParsedAttribute parsed = parse(context.attrKey());

        ObligationDefinition definition =
                readDefinition(parsed.obligationId());

        EvidenceRow evidence = switch (definition.scope()) {

            case "USER_GLOBAL" ->
                    findUserGlobalEvidence(
                            definition.obligationId(),
                            definition.version(),
                            context.request().subjectUserId()
                    );

            case "USER_POLICY" ->
                    findUserPolicyEvidence(
                            definition.obligationId(),
                            definition.version(),
                            context.request().subjectUserId(),
                            context.policyRef()
                    );

            default -> throw new IllegalArgumentException(
                    "Unsupported obligation scope: "
                            + definition.scope()
            );
        };

        return readField(
                parsed.field(),
                evidence
        );
    }

    private ObligationDefinition readDefinition(
            String obligationId
    ) {
        return jdbcTemplate.queryForObject(
                """
                SELECT obligation_id,
                       version,
                       scope
                FROM obligation_catalog
                WHERE obligation_id = ?
                  AND enabled = true
                """,
                (rs, rowNum) -> new ObligationDefinition(
                        rs.getString("obligation_id"),
                        rs.getString("version"),
                        rs.getString("scope")
                ),
                obligationId
        );
    }

    private EvidenceRow findUserGlobalEvidence(
            String obligationId,
            String version,
            Long subjectUserId
    ) {
        List<EvidenceRow> rows = jdbcTemplate.query(
                """
                SELECT status,
                       obligation_version
                FROM obligation_evidence
                WHERE obligation_id = ?
                  AND obligation_version = ?
                  AND subject_user_id = ?
                  AND policy_ref IS NULL
                ORDER BY fulfilled_at DESC, id DESC
                LIMIT 1
                """,
                (rs, rowNum) -> new EvidenceRow(
                        rs.getString("status"),
                        rs.getString("obligation_version")
                ),
                obligationId,
                version,
                subjectUserId
        );

        return firstOrNull(rows);
    }

    private EvidenceRow findUserPolicyEvidence(
            String obligationId,
            String version,
            Long subjectUserId,
            String policyRef
    ) {
        if (policyRef == null || policyRef.isBlank()) {
            throw new IllegalArgumentException(
                    "policyRef is required for USER_POLICY scope"
            );
        }

        List<EvidenceRow> rows = jdbcTemplate.query(
                """
                SELECT status,
                       obligation_version
                FROM obligation_evidence
                WHERE obligation_id = ?
                  AND obligation_version = ?
                  AND subject_user_id = ?
                  AND policy_ref = ?
                ORDER BY fulfilled_at DESC, id DESC
                LIMIT 1
                """,
                (rs, rowNum) -> new EvidenceRow(
                        rs.getString("status"),
                        rs.getString("obligation_version")
                ),
                obligationId,
                version,
                subjectUserId,
                policyRef
        );

        return firstOrNull(rows);
    }

    private EvidenceRow firstOrNull(
            List<EvidenceRow> rows
    ) {
        return rows.isEmpty()
                ? null
                : rows.get(0);
    }

    private Object readField(
            String field,
            EvidenceRow evidence
    ) {
        return switch (field) {

            case "status" ->
                    evidence == null
                            ? "MISSING"
                            : evidence.status();

            case "version" ->
                    evidence == null
                            ? "NONE"
                            : evidence.version();

            default -> throw new IllegalArgumentException(
                    "Unsupported obligation evidence field: "
                            + field
            );
        };
    }

    private ParsedAttribute parse(String attrKey) {
        if (attrKey == null
                || !attrKey.startsWith(ATTR_PREFIX)) {
            throw new IllegalArgumentException(
                    "Invalid obligation attribute key: "
                            + attrKey
            );
        }

        int lastDot = attrKey.lastIndexOf('.');

        if (lastDot <= ATTR_PREFIX.length()
                || lastDot == attrKey.length() - 1) {
            throw new IllegalArgumentException(
                    "Invalid obligation attribute key: "
                            + attrKey
            );
        }

        String obligationId = attrKey.substring(
                ATTR_PREFIX.length(),
                lastDot
        );

        String field = attrKey.substring(
                lastDot + 1
        );

        return new ParsedAttribute(
                obligationId,
                field
        );
    }

    private record ParsedAttribute(
            String obligationId,
            String field
    ) {
    }

    private record ObligationDefinition(
            String obligationId,
            String version,
            String scope
    ) {
    }

    private record EvidenceRow(
            String status,
            String version
    ) {
    }
}