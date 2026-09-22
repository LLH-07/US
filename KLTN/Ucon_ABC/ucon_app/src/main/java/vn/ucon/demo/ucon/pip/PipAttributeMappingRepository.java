package vn.ucon.demo.ucon.pip;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;
import vn.ucon.demo.ucon.model.PipAttributeMapping;

@Repository
public class PipAttributeMappingRepository {

    private final JdbcTemplate jdbcTemplate;

    public PipAttributeMappingRepository(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public PipAttributeMapping findByAttrKey(String attrKey) {
        return jdbcTemplate.queryForObject("""
            SELECT attr_key,
                   xacml_attribute_id,
                   xacml_category,
                   xacml_data_type,
                   reader_name,
                   writer_name,
                   dependency_name,
                   monitor_type,
                   mutable,
                   enabled
            FROM pip_attribute_mappings
            WHERE attr_key = ?
              AND enabled = true
        """, (rs, rowNum) -> new PipAttributeMapping(
                rs.getString("attr_key"),
                rs.getString("xacml_attribute_id"),
                rs.getString("xacml_category"),
                rs.getString("xacml_data_type"),
                rs.getString("reader_name"),
                rs.getString("writer_name"),
                rs.getString("dependency_name"),
                rs.getString("monitor_type"),
                rs.getBoolean("mutable"),
                rs.getBoolean("enabled")
        ), attrKey);
    }
}
