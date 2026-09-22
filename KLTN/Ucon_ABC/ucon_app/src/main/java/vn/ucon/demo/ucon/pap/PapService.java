package vn.ucon.demo.ucon.pap;

import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import vn.ucon.demo.ucon.model.PolicyAttributeRequirement;
import vn.ucon.demo.ucon.model.PolicyMetadata;

import java.util.List;
import java.util.Map;

@Service
public class PapService {

    private final JdbcTemplate jdbcTemplate;

    public PapService(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public PolicyMetadata findPolicyMetadata(
            String resourceType,
            String actionId,
            String accessPath,
            String phase
    ) {
        Map<String, Object> policyRow;

        try {
            policyRow = jdbcTemplate.queryForMap("""
                SELECT id,
                       policy_ref,
                       policy_name,
                       resource_type,
                       action_id,
                       access_path,
                       phase
                FROM policy_registry
                WHERE resource_type = ?
                  AND action_id = ?
                  AND access_path = ?
                  AND phase = ?
                  AND enabled = true
                LIMIT 1
            """, resourceType, actionId, accessPath, phase);
        } catch (EmptyResultDataAccessException e) {
            throw new IllegalStateException(
                    "No enabled policy found for resourceType=" + resourceType
                            + ", actionId=" + actionId
                            + ", accessPath=" + accessPath
                            + ", phase=" + phase,
                    e
            );
        }

        Long policyId = ((Number) policyRow.get("id")).longValue();

        List<PolicyAttributeRequirement> attrReqs = jdbcTemplate.query("""
            SELECT id,
                   policy_id,
                   attr_key,
                   usage_type,
                   required
            FROM policy_attribute_requirements
            WHERE policy_id = ?
            ORDER BY id
        """, (rs, rowNum) -> new PolicyAttributeRequirement(
                rs.getLong("id"),
                rs.getLong("policy_id"),
                rs.getString("attr_key"),
                rs.getString("usage_type"),
                rs.getBoolean("required")
        ), policyId);

        return new PolicyMetadata(
                policyId,
                (String) policyRow.get("policy_ref"),
                (String) policyRow.get("policy_name"),
                (String) policyRow.get("resource_type"),
                (String) policyRow.get("action_id"),
                (String) policyRow.get("access_path"),
                (String) policyRow.get("phase"),
                attrReqs
        );
    }
}