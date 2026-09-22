package vn.ucon.demo.ucon.pip.provider.reader;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Component;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;

@Component
public class EbookResourceAttributeReader implements AttributeReader {

    private final JdbcTemplate jdbcTemplate;

    public EbookResourceAttributeReader(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public boolean supports(String readerName) {
        return "EbookResourceAttributeReader".equals(readerName);
    }

    @Override
    public Object read(AttributeReadContext context) {
        return switch (context.attrKey()) {
            // New schema-based attribute keys
            case "book_volumes.status" -> readBookVolumeStatus(context);
            case "books.status" -> readBookStatus(context);
            case "book_volumes.access_model_override" -> readBookVolumeAccessModelOverride(context);
            case "book_volumes.read_count" -> readBookVolumeReadCount(context);

            default -> throw new IllegalArgumentException(
                    "Unsupported ebook resource attribute: " + context.attrKey()
            );
        };
    }

    private String readBookVolumeStatus(AttributeReadContext context) {
        return jdbcTemplate.queryForObject("""
            SELECT status
            FROM book_volumes
            WHERE id = ?
        """, String.class, context.request().resourceId());
    }

    private String readBookStatus(AttributeReadContext context) {
        return jdbcTemplate.queryForObject("""
            SELECT b.status
            FROM books b
            JOIN book_volumes bv ON bv.book_id = b.id
            WHERE bv.id = ?
        """, String.class, context.request().resourceId());
    }

    private String readBookVolumeAccessModelOverride(AttributeReadContext context) {
        return jdbcTemplate.queryForObject("""
            SELECT access_model_override
            FROM book_volumes
            WHERE id = ?
        """, String.class, context.request().resourceId());
    }

    private Long readBookVolumeReadCount(AttributeReadContext context) {
        return jdbcTemplate.queryForObject("""
            SELECT read_count
            FROM book_volumes
            WHERE id = ?
        """, Long.class, context.request().resourceId());
    }
}
