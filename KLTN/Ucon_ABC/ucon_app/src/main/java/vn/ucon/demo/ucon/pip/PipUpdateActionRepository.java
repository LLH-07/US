package vn.ucon.demo.ucon.pip;

import java.util.List;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import vn.ucon.demo.ucon.model.PipUpdateAction;

@Repository
public class PipUpdateActionRepository {

    private final JdbcTemplate jdbcTemplate;

    public PipUpdateActionRepository(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public PipUpdateAction findByActionCode(String actionCode) {
        String normalizedActionCode = actionCode == null ? null : actionCode.trim();

        List<PipUpdateAction> results = jdbcTemplate.query("""
            SELECT action_code,
                attr_key,
                handler_name,
                operation,
                default_value,
                enabled
            FROM pip_update_actions
            WHERE action_code = ?
            AND enabled = true
        """, (rs, rowNum) -> new PipUpdateAction(
                rs.getString("action_code"),
                rs.getString("attr_key"),
                rs.getString("handler_name"),
                rs.getString("operation"),
                rs.getString("default_value"),
                rs.getBoolean("enabled")
        ), normalizedActionCode);

        if (results.isEmpty()) {
            throw new IllegalArgumentException(
                    "Unsupported update actionCode: [" + normalizedActionCode + "]"
            );
        }

        return results.get(0);
    }
}