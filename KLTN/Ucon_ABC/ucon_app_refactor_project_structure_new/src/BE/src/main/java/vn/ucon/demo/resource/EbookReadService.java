package vn.ucon.demo.resource;

import org.springframework.core.io.Resource;
import org.springframework.core.io.ResourceLoader;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.http.HttpStatus;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.File;
import java.io.InputStream;
// import java.sql.Timestamp;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.HashSet;
import java.util.Set;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
// import java.time.LocalDateTime;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

@Service
public class EbookReadService {

    private static final int DEFAULT_CHUNK_SIZE = 3400;
    private static final int DEFAULT_PAGES_PER_VOLUME = 20;

    private final JdbcTemplate jdbcTemplate;
    private final ResourceLoader resourceLoader;
    private final Map<String, List<String>> serverTextCache = new ConcurrentHashMap<>();
    private final Map<String, EpubMeta> epubMetaCache = new ConcurrentHashMap<>();
    private final Map<String, ContentAvailability> contentAvailabilityCache = new ConcurrentHashMap<>();

    public EbookReadService(JdbcTemplate jdbcTemplate, ResourceLoader resourceLoader) {
        this.jdbcTemplate = jdbcTemplate;
        this.resourceLoader = resourceLoader;
    }


    private Long numberAsLong(Object value) {
        return value instanceof Number number ? number.longValue() : null;
    }

    private int numberAsInt(Object value, int fallback) {
        if (value instanceof Number number) return number.intValue();
        try {
            return value == null ? fallback : Integer.parseInt(String.valueOf(value));
        } catch (Exception ex) {
            return fallback;
        }
    }

    private long nextId(String tableName) {
        String safeTable = tableName.replaceAll("[^a-zA-Z0-9_]", "");
        Long value = jdbcTemplate.queryForObject("SELECT COALESCE(MAX(id), 0) + 1 FROM " + safeTable, Long.class);
        return value == null ? 1L : value;
    }

    public String effectiveAccessModelForBook(Long bookId) {
        try {
            String model = jdbcTemplate.queryForObject(
                    """
                    SELECT COALESCE(bv.access_model_override, b.default_access_model)
                    FROM books b
                    JOIN book_volumes bv ON bv.book_id = b.id
                    WHERE b.id = ?
                      AND b.status = 'ACTIVE'
                      AND bv.status = 'ACTIVE'
                    ORDER BY bv.volume_no, bv.id
                    LIMIT 1
                    """,
                    String.class,
                    bookId
            );
            return model == null || model.isBlank() ? "FREE" : model.trim().toUpperCase(Locale.ROOT);
        } catch (EmptyResultDataAccessException ex) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "BOOK_NOT_FOUND");
        }
    }

    public void syncCatalogFromFolders() {
        ensureEpubCatalogFromFolders();
        serverTextCache.clear();
        epubMetaCache.clear();
        contentAvailabilityCache.clear();
    }

    private void ensureEpubCatalogFromFolders() {
        List<DiscoveredEpub> discovered = discoveredEpubFiles();
        if (discovered.isEmpty()) return;

        Set<String> knownPaths = new HashSet<>(jdbcTemplate.queryForList(
                """
                SELECT DISTINCT content_path
                FROM book_volumes
                WHERE content_path IS NOT NULL
                  AND content_path <> ''
                """,
                String.class
        ));
        Set<String> knownFileNames = new HashSet<>();
        for (String knownPath : knownPaths) {
            knownFileNames.add(cleanFileName(knownPath).toLowerCase(Locale.ROOT));
        }

        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));

        for (DiscoveredEpub item : discovered) {
            String path = item.catalogPath();
            String fileKey = cleanFileName(item.fileName()).toLowerCase(Locale.ROOT);
            if (knownPaths.contains(path) || knownFileNames.contains(fileKey)) continue;

            List<String> chunks = chunksForPath(path, chunkSize);
            if (!hasReadableContent(chunks)) continue;

            EpubMeta meta = metaForEpub(path);
            String accessModel = accessModelForImportedFile(item.fileName());
            long bookId = nextId("books");
            int totalVolumes = Math.max(1, (int) Math.ceil(chunks.size() / (double) pagesPerVolume));
            int price = "PAID".equals(accessModel) ? 99000 : 0;

            jdbcTemplate.update(
                    """
                    INSERT INTO books(id, title, genre, authors, default_access_model, status)
                    VALUES (?, ?, ?, ?, ?, 'ACTIVE')
                    """,
                    bookId,
                    meta.title(),
                    "ebook-import",
                    meta.authors(),
                    accessModel
            );


            for (int volumeNo = 1; volumeNo <= totalVolumes; volumeNo++) {
                jdbcTemplate.update(
                        """
                        INSERT INTO book_volumes(id, book_id, title, volume_no, price, content_path, access_model_override, status)
                        VALUES (?, ?, ?, ?, ?, ?, ?, 'ACTIVE')
                        """,
                        nextId("book_volumes"),
                        bookId,
                        meta.title() + " - Tập " + String.format(Locale.ROOT, "%02d", volumeNo),
                        volumeNo,
                        price,
                        path,
                        accessModel
                );
            }

            knownPaths.add(path);
            knownFileNames.add(fileKey);
        }
    }

    private List<DiscoveredEpub> discoveredEpubFiles() {
        Map<String, DiscoveredEpub> result = new LinkedHashMap<>();
        collectEpubFilesFromDirectory("classpath", classpathEbooksDirectory(), result);
        collectEpubFilesFromDirectory("file", new File("ebooks"), result);
        return new ArrayList<>(result.values());
    }

    private File classpathEbooksDirectory() {
        try {
            Resource dir = resourceLoader.getResource("classpath:ebooks/");
            if (dir.exists()) return dir.getFile();
        } catch (IOException ignored) {
            // When packaged as a jar, classpath resources may not be represented as normal files.
        }
        return null;
    }

    private void collectEpubFilesFromDirectory(String source, File directory, Map<String, DiscoveredEpub> result) {
        if (directory == null || !directory.exists() || !directory.isDirectory()) return;
        File[] files = directory.listFiles(file -> file.isFile() && file.getName().toLowerCase(Locale.ROOT).endsWith(".epub"));
        if (files == null || files.length == 0) return;
        List<File> sorted = new ArrayList<>(List.of(files));
        sorted.sort(Comparator.comparing(File::getName));
        for (File file : sorted) {
            String fileName = file.getName();
            String path = "classpath".equals(source)
                    ? "ebooks/" + fileName
                    : "file:ebooks/" + fileName;
            result.putIfAbsent(path, new DiscoveredEpub(path, fileName));
        }
    }

    private String accessModelForImportedFile(String fileName) {
        String lower = fileName == null ? "" : fileName.toLowerCase(Locale.ROOT).trim();
        if (lower.startsWith("free-") || lower.startsWith("[free]") || lower.contains("__free__")) return "FREE";
        if (lower.startsWith("rental-") || lower.startsWith("[rental]") || lower.contains("__rental__")) return "RENTAL";
        if (lower.startsWith("paid-") || lower.startsWith("[paid]") || lower.contains("__paid__")) return "PAID";
        return "PAID";
    }

    public Map<String, Object> catalog() {
        // Catalog chỉ trả về sách thật sự đọc được.
        // Nếu EPUB không trích xuất được nội dung, hoặc tập vượt quá số chunk thực tế, backend không đưa lên thư viện.
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));

        List<Map<String, Object>> allBooks = jdbcTemplate.queryForList(
                """
                SELECT id, title, genre, authors, default_access_model, status
                FROM books
                WHERE status = 'ACTIVE'
                ORDER BY id
                """
        );

        List<Map<String, Object>> allVolumes = jdbcTemplate.queryForList(
                """
                SELECT bv.id, bv.book_id, bv.title, bv.volume_no, bv.price, bv.content_path, bv.access_model_override, bv.status,
                       COALESCE(bv.read_count, 0) AS read_count
                FROM book_volumes bv
                JOIN books b ON b.id = bv.book_id
                WHERE bv.status = 'ACTIVE'
                  AND b.status = 'ACTIVE'
                ORDER BY bv.book_id, bv.volume_no
                """
        );

        Map<Long, List<Map<String, Object>>> volumesByBook = new LinkedHashMap<>();
        for (Map<String, Object> volume : allVolumes) {
            Long bookId = numberAsLong(volume.get("book_id"));
            if (bookId == null) continue;
            volumesByBook.computeIfAbsent(bookId, ignored -> new ArrayList<>()).add(volume);
        }

        List<Map<String, Object>> catalogBooks = new ArrayList<>();
        List<Map<String, Object>> catalogVolumes = new ArrayList<>();

        for (Map<String, Object> book : allBooks) {
            Long bookId = numberAsLong(book.get("id"));
            if (bookId == null) continue;

            List<Map<String, Object>> dbVolumes = volumesByBook.getOrDefault(bookId, List.of());
            if (dbVolumes.isEmpty()) continue;

            String path = dbVolumes.stream()
                    .map(volume -> String.valueOf(volume.getOrDefault("content_path", "")).trim())
                    .filter(value -> !value.isBlank())
                    .findFirst()
                    .orElse(null);
            if (path == null || !resourceExistsForPath(path)) continue;

            ContentAvailability availability = contentAvailabilityForPath(path, chunkSize, pagesPerVolume);
            if (!availability.available()) continue;

            int readableVolumeCount = 0;
            for (Map<String, Object> volume : dbVolumes) {
                int volumeNo = numberAsInt(volume.get("volume_no"), 1);
                if (volumeNo < 1 || volumeNo > availability.totalVolumes()) continue;

                int chunksInThisVolume = chunksInVolume(volumeNo, availability.totalChunks(), pagesPerVolume);
                if (chunksInThisVolume <= 0) continue;

                Map<String, Object> safeVolume = new LinkedHashMap<>(volume);
                safeVolume.put("title", "Tập " + String.format(Locale.ROOT, "%02d", volumeNo));
                safeVolume.put("volume_no", volumeNo);
                safeVolume.put("contentAvailable", true);
                safeVolume.put("totalChunks", chunksInThisVolume);
                catalogVolumes.add(safeVolume);
                readableVolumeCount++;
            }

            if (readableVolumeCount == 0) continue;

            Map<String, Object> safeBook = new LinkedHashMap<>(book);
            safeBook.put("contentAvailable", true);
            safeBook.put("readableVolumeCount", readableVolumeCount);
            safeBook.put("totalBookChunks", availability.totalChunks());
            safeBook.put("totalVolumes", availability.totalVolumes());
            catalogBooks.add(safeBook);
        }

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("books", catalogBooks);
        result.put("volumes", catalogVolumes);
        return result;
    }

    private int chunksInVolume(int volumeNo, int totalBookChunks, int pagesPerVolume) {
        int startIndex = (Math.max(1, volumeNo) - 1) * pagesPerVolume;
        int remaining = Math.max(0, totalBookChunks - startIndex);
        return Math.min(pagesPerVolume, remaining);
    }

    private ContentAvailability contentAvailabilityForPath(String path, int chunkSize, int pagesPerVolume) {
        String key = path + "::" + chunkSize + "::" + pagesPerVolume;
        return contentAvailabilityCache.computeIfAbsent(key, ignored -> {
            List<String> chunks = chunksForPath(path, chunkSize);
            int totalCharacters = chunks == null ? 0 : chunks.stream().mapToInt(String::length).sum();
            boolean available = chunks != null && !chunks.isEmpty() && totalCharacters >= 500;
            int totalChunks = chunks == null ? 0 : chunks.size();
            int totalVolumes = available ? Math.max(1, (int) Math.ceil(totalChunks / (double) pagesPerVolume)) : 0;
            return new ContentAvailability(available, totalChunks, totalCharacters, totalVolumes);
        });
    }

    public boolean isReadableBook(Long bookId) {
        if (bookId == null) return false;
        String path = contentPathForBook(bookId);
        if (path == null || !resourceExistsForPath(path)) return false;
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        return contentAvailabilityForPath(path, chunkSize, pagesPerVolume).available();
    }

    public List<Map<String, Object>> readablePurchasedLibraryRows(List<Map<String, Object>> rows) {
        List<Map<String, Object>> readable = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            Long bookId = numberAsLong(row.get("book_id"));
            if (isReadableBook(bookId)) readable.add(row);
        }
        return readable;
    }

    public List<Map<String, Object>> readablePaidVolumeRowsForBook(Long bookId) {
        if (!isReadableBook(bookId)) return List.of();
        String path = contentPathForBook(bookId);
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        ContentAvailability availability = contentAvailabilityForPath(path, chunkSize, pagesPerVolume);

        List<Map<String, Object>> rows = jdbcTemplate.queryForList(
                """
                SELECT bv.id, bv.volume_no, COALESCE(bv.price, 0) AS price
                FROM book_volumes bv
                JOIN books b ON b.id = bv.book_id
                WHERE bv.book_id = ?
                  AND bv.status = 'ACTIVE'
                  AND b.status = 'ACTIVE'
                  AND COALESCE(bv.access_model_override, b.default_access_model) = 'PAID'
                ORDER BY bv.volume_no, bv.id
                """,
                bookId
        );

        List<Map<String, Object>> readable = new ArrayList<>();
        for (Map<String, Object> row : rows) {
            int volumeNo = numberAsInt(row.get("volume_no"), 1);
            if (volumeNo >= 1 && volumeNo <= availability.totalVolumes() && chunksInVolume(volumeNo, availability.totalChunks(), pagesPerVolume) > 0) {
                readable.add(row);
            }
        }
        return readable;
    }

    public List<Long> readablePaidVolumeIdsForBook(Long bookId) {
        List<Long> ids = new ArrayList<>();
        for (Map<String, Object> row : readablePaidVolumeRowsForBook(bookId)) {
            Long id = numberAsLong(row.get("id"));
            if (id != null) ids.add(id);
        }
        return ids;
    }


    private String contentPathForBook(Long bookId) {
        if (bookId == null) return null;
        try {
            String path = jdbcTemplate.queryForObject(
                    """
                    SELECT content_path
                    FROM book_volumes
                    WHERE book_id = ?
                      AND status = 'ACTIVE'
                      AND content_path IS NOT NULL
                      AND content_path <> ''
                    ORDER BY volume_no, id
                    LIMIT 1
                    """,
                    String.class,
                    bookId
            );
            return path == null || path.isBlank() ? null : path.trim();
        } catch (EmptyResultDataAccessException ex) {
            return null;
        }
    }

    private boolean resourceExistsForPath(String path) {
        if (path == null || path.isBlank()) return false;
        String normalized = path.startsWith("classpath:") || path.startsWith("file:")
                ? path
                : "classpath:" + path;
        try {
            return resourceLoader.getResource(normalized).exists();
        } catch (Exception ex) {
            return false;
        }
    }

    // private Long firstVolumeIdForBook(Long bookId) {
    //     try {
    //         return jdbcTemplate.queryForObject(
    //                 "SELECT MIN(id) FROM book_volumes WHERE book_id = ? AND status = 'ACTIVE'",
    //                 Long.class,
    //                 bookId
    //         );
    //     } catch (EmptyResultDataAccessException ex) {
    //         return null;
    //     }
    // }

    public ChunkPayload fetchChunk(Long sessionId, Long fallbackBookVolumeId, int chunkNo) {
        SessionInfo session = resolveSession(sessionId, fallbackBookVolumeId);
        Long bookVolumeId = session.bookVolumeId() != null ? session.bookVolumeId() : fallbackBookVolumeId;
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);

        String accessPath = sessionAccessPath(sessionId);
        int safeChunkNo = Math.max(1, chunkNo);
        if ("PREVIEW".equalsIgnoreCase(accessPath) && safeChunkNo > 5) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "PREVIEW_LIMIT_REACHED");
        }

        int realIndex = metadata.startChunkIndex() + safeChunkNo - 1;
        String content = "";
        if (safeChunkNo <= metadata.totalChunks() && realIndex >= 0 && realIndex < metadata.allBookChunks().size()) {
            content = metadata.allBookChunks().get(realIndex);
        }

        return new ChunkPayload(
                sessionId,
                session.userId(),
                bookVolumeId,
                safeChunkNo,
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                content
        );
    }

    public ChunkPayload fetchPreviewChunk(Long userId, Long bookVolumeId, int chunkNo) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        int safeChunkNo = Math.max(1, chunkNo);
        if (safeChunkNo > 5) {
            throw new ResponseStatusException(HttpStatus.FORBIDDEN, "PREVIEW_LIMIT_REACHED");
        }

        int realIndex = metadata.startChunkIndex() + safeChunkNo - 1;
        String content = "";
        if (safeChunkNo <= metadata.totalChunks() && realIndex >= 0 && realIndex < metadata.allBookChunks().size()) {
            content = metadata.allBookChunks().get(realIndex);
        }

        return new ChunkPayload(
                null,
                userId,
                bookVolumeId,
                safeChunkNo,
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                content
        );
    }

    private String sessionAccessPath(Long sessionId) {
        if (sessionId == null) return "";
        try {
            String accessPath = jdbcTemplate.queryForObject(
                    "SELECT access_path FROM usage_sessions WHERE id = ?",
                    String.class,
                    sessionId
            );
            return accessPath == null ? "" : accessPath;
        } catch (EmptyResultDataAccessException ex) {
            return "";
        }
    }

    public ReaderMetadata metadataForVolumePublic(Long bookVolumeId) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        return new ReaderMetadata(
                bookVolumeId,
                metadata.bookId(),
                metadata.volumeNo(),
                metadata.bookTitle(),
                metadata.authors(),
                metadata.volumeTitle(),
                metadata.totalChunks(),
                metadata.totalBookChunks(),
                metadata.totalVolumes(),
                metadata.pagesPerVolume(),
                metadata.contentAvailable()
        );
    }

    public List<Map<String, Object>> readHistoryForUser(Long userId, int days, int limit) {
        int safeDays = Math.max(1, Math.min(days, 60));
        int safeLimit = Math.max(1, Math.min(limit, 50));
        return jdbcTemplate.queryForList(
                """
                WITH ranked AS (
                    SELECT
                        us.id,
                        us.subject_user_id,
                        us.resource_type,
                        us.resource_id,
                        us.resource_id AS book_volume_id,
                        us.action_id,
                        us.access_path,
                        us.phase,
                        us.status,
                        to_char(us.created_at, 'YYYY-MM-DD HH24:MI:SS') AS created_at,
                        to_char(us.activated_at, 'YYYY-MM-DD HH24:MI:SS') AS activated_at,
                        to_char(us.last_eval_at, 'YYYY-MM-DD HH24:MI:SS') AS last_eval_at,
                        to_char(us.ended_at, 'YYYY-MM-DD HH24:MI:SS') AS ended_at,
                        to_char(us.revoked_at, 'YYYY-MM-DD HH24:MI:SS') AS revoked_at,
                        us.revoked_reason,
                        to_char(us.post_handled_at, 'YYYY-MM-DD HH24:MI:SS') AS post_handled_at,
                        bv.book_id AS book_id,
                        bv.volume_no AS volume_no,
                        bv.title AS volume_title,
                        b.title AS book_title,
                        b.authors AS authors,
                        ROW_NUMBER() OVER (
                            PARTITION BY b.id
                            ORDER BY COALESCE(us.ended_at, us.post_handled_at, us.last_eval_at, us.activated_at, us.created_at) DESC
                        ) AS rn,
                        COALESCE(us.ended_at, us.post_handled_at, us.last_eval_at, us.activated_at, us.created_at) AS sort_at
                    FROM usage_sessions us
                    JOIN book_volumes bv ON bv.id = us.resource_id
                    JOIN books b ON b.id = bv.book_id
                    WHERE us.subject_user_id = ?
                      AND us.resource_type = 'BOOK_VOLUME'
                      AND us.action_id = 'READ'
                      AND us.status = 'ENDED'
                      AND us.created_at >= now() - (? * interval '1 day')
                )
                SELECT
                    id,
                    subject_user_id,
                    resource_type,
                    resource_id,
                    book_volume_id,
                    action_id,
                    access_path,
                    phase,
                    status,
                    created_at,
                    activated_at,
                    last_eval_at,
                    ended_at,
                    revoked_at,
                    revoked_reason,
                    post_handled_at,
                    book_id,
                    volume_no,
                    volume_title,
                    book_title,
                    authors
                FROM ranked
                WHERE rn = 1
                ORDER BY sort_at DESC
                LIMIT ?
                """,
                userId,
                safeDays,
                safeLimit
        );
    }

    public String syncBookTitleFromEpub(Long bookVolumeId) {
        VolumeContentMetadata metadata = metadataForVolume(bookVolumeId);
        if (metadata.bookId() == null || !metadata.contentAvailable()) {
            return metadata.bookTitle();
        }

        jdbcTemplate.update(
                "UPDATE books SET title = ?, authors = ? WHERE id = ?",
                metadata.bookTitle(),
                metadata.authors(),
                metadata.bookId()
        );
        jdbcTemplate.update(
                """
                UPDATE book_volumes
                SET title = 'T\u1eadp ' || LPAD(volume_no::text, 2, '0')
                WHERE book_id = ?
                """,
                metadata.bookId()
        );
        return metadata.bookTitle();
    }

    public void syncAllBookMetadataFromEpub() {
        List<Long> volumeIds = jdbcTemplate.queryForList(
                """
                SELECT MIN(id) AS id
                FROM book_volumes
                GROUP BY book_id
                ORDER BY MIN(book_id)
                """,
                Long.class
        );
        for (Long volumeId : volumeIds) {
            syncBookTitleFromEpub(volumeId);
        }
    }

    private SessionInfo resolveSession(Long sessionId, Long fallbackBookVolumeId) {
        if (sessionId == null) {
            return new SessionInfo(null, fallbackBookVolumeId);
        }
        try {
            return jdbcTemplate.queryForObject(
                    """
                    SELECT subject_user_id, resource_id
                    FROM usage_sessions
                    WHERE id = ?
                    """,
                    (rs, rowNum) -> new SessionInfo(
                            rs.getLong("subject_user_id"),
                            rs.getLong("resource_id")
                    ),
                    sessionId
            );
        } catch (EmptyResultDataAccessException ex) {
            return new SessionInfo(null, fallbackBookVolumeId);
        }
    }

    private VolumeInfo volumeInfo(Long bookVolumeId) {
        if (bookVolumeId == null) {
            return new VolumeInfo(null, null, 1, "T\u1eadp 01", null);
        }
        try {
            return jdbcTemplate.queryForObject(
                    """
                    SELECT bv.id, bv.book_id, bv.volume_no, bv.title, bv.content_path
                    FROM book_volumes bv
                    WHERE bv.id = ?
                    """,
                    (rs, rowNum) -> new VolumeInfo(
                            rs.getLong("id"),
                            rs.getLong("book_id"),
                            rs.getInt("volume_no"),
                            rs.getString("title"),
                            rs.getString("content_path")
                    ),
                    bookVolumeId
            );
        } catch (EmptyResultDataAccessException ex) {
            return new VolumeInfo(bookVolumeId, null, Math.max(1, bookVolumeId.intValue()), "T\u1eadp " + bookVolumeId, null);
        }
    }

    private VolumeContentMetadata metadataForVolume(Long bookVolumeId) {
        VolumeInfo volume = volumeInfo(bookVolumeId);
        String path = contentPathForVolume(volume);
        int chunkSize = intParameterValue("BOOK_CONTENT_CHUNK_SIZE", DEFAULT_CHUNK_SIZE);
        int pagesPerVolume = Math.max(1, intParameterValue("BOOK_VOLUME_PAGE_COUNT", DEFAULT_PAGES_PER_VOLUME));
        List<String> allChunks = chunksForPath(path, chunkSize);

        int totalBookChunks = allChunks.size();
        int totalVolumes = Math.max(1, (int) Math.ceil(totalBookChunks / (double) pagesPerVolume));
        int volumeNo = Math.max(1, volume.volumeNo());
        int startChunkIndex = (volumeNo - 1) * pagesPerVolume;
        int remaining = Math.max(0, totalBookChunks - startChunkIndex);
        int totalChunksInVolume = Math.min(pagesPerVolume, remaining);
        if (totalChunksInVolume == 0 && totalBookChunks > 0 && volumeNo == 1) {
            totalChunksInVolume = Math.min(pagesPerVolume, totalBookChunks);
        }

        EpubMeta meta = metaForEpub(path);
        String volumeTitle = "T\u1eadp " + String.format(Locale.ROOT, "%02d", volumeNo);

        return new VolumeContentMetadata(
                volume.bookId(),
                volume.id(),
                volumeNo,
                meta.title(),
                meta.authors(),
                volumeTitle,
                allChunks,
                startChunkIndex,
                totalChunksInVolume,
                totalBookChunks,
                totalVolumes,
                pagesPerVolume,
                hasReadableContent(allChunks) && totalChunksInVolume > 0
        );
    }

    private boolean hasReadableContent(List<String> chunks) {
        if (chunks == null || chunks.isEmpty()) return false;
        int totalCharacters = chunks.stream().mapToInt(String::length).sum();
        return totalCharacters >= 500;
    }

    private String contentPathForVolume(VolumeInfo volume) {
        if (volume == null || volume.contentPath() == null || volume.contentPath().isBlank()) return "";
        return volume.contentPath().trim();
    }

    private List<String> chunksForPath(String path, int chunkSize) {
        String cacheKey = path + "::" + chunkSize;
        return serverTextCache.computeIfAbsent(cacheKey, ignored -> splitIntoChunks(readEpubText(path), chunkSize));
    }

    private String parameterValue(String key, String defaultValue) {
        try {
            String value = jdbcTemplate.queryForObject(
                    "SELECT value FROM system_parameters WHERE parameter_name = ?",
                    String.class,
                    key
            );
            return value == null || value.isBlank() ? defaultValue : value.trim();
        } catch (EmptyResultDataAccessException ex) {
            return defaultValue;
        }
    }

    private int intParameterValue(String key, int defaultValue) {
        try {
            return Integer.parseInt(parameterValue(key, String.valueOf(defaultValue)));
        } catch (NumberFormatException ex) {
            return defaultValue;
        }
    }

    private String readEpubText(String classpathLocation) {
        if (classpathLocation == null || classpathLocation.isBlank()) return "";
        String normalized = classpathLocation.startsWith("classpath:") || classpathLocation.startsWith("file:")
                ? classpathLocation
                : "classpath:" + classpathLocation;
        Resource resource = resourceLoader.getResource(normalized);

        if (!resource.exists()) {
            return "";
        }

        // Một số EPUB có tên entry/encoding khác UTF-8. Thử UTF-8 trước, sau đó fallback ISO-8859-1
        // để tránh tình trạng sách hợp lệ nhưng backend trích xuất rỗng.
        for (Charset charset : List.of(StandardCharsets.UTF_8, StandardCharsets.ISO_8859_1)) {
            try {
                String text = readEpubTextWithCharset(resource, charset);
                if (!text.isBlank()) return text;
            } catch (Exception ignored) {
                // thử charset tiếp theo
            }
        }
        return "";
    }

    private String readEpubTextWithCharset(Resource resource, Charset charset) throws IOException {
        try (InputStream inputStream = resource.getInputStream();
             ZipInputStream zip = new ZipInputStream(inputStream, charset)) {

            EpubPackage epubPackage = readEpubPackage(resource);
            Map<String, byte[]> htmlFiles = new LinkedHashMap<>();
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null) {
                String name = entry.getName();
                String lower = name.toLowerCase(Locale.ROOT);
                if (entry.isDirectory()) continue;
                if (!(lower.endsWith(".xhtml") || lower.endsWith(".html") || lower.endsWith(".htm"))) continue;
                if (lower.contains("nav") || lower.contains("toc") || lower.contains("cover")) continue;
                htmlFiles.put(normalizePath(name), readCurrentZipEntry(zip));
            }

            List<String> orderedNames = epubPackage.spineHtmlPaths().isEmpty()
                    ? new ArrayList<>(htmlFiles.keySet())
                    : epubPackage.spineHtmlPaths();

            List<EpubSection> sections = new ArrayList<>();
            for (String sectionName : orderedNames) {
                byte[] bytes = htmlFiles.get(normalizePath(sectionName));
                if (bytes == null) continue;
                String html = decodeTextBytes(bytes);
                String text = cleanHtml(html);
                if (!text.isBlank()) {
                    sections.add(new EpubSection(sectionName, text));
                }
            }

            if (sections.isEmpty()) {
                htmlFiles.entrySet().stream()
                        .sorted(Map.Entry.comparingByKey())
                        .forEach(htmlEntry -> {
                            String text = cleanHtml(decodeTextBytes(htmlEntry.getValue()));
                            if (!text.isBlank()) sections.add(new EpubSection(htmlEntry.getKey(), text));
                        });
            }

            return String.join("\n\n", sections.stream().map(EpubSection::text).toList()).trim();
        }
    }

    private EpubMeta metaForEpub(String classpathLocation) {
        return epubMetaCache.computeIfAbsent(classpathLocation, this::extractMetaFromEpubOrFileName);
    }

    private EpubMeta extractMetaFromEpubOrFileName(String classpathLocation) {
        if (classpathLocation == null || classpathLocation.isBlank()) {
            return new EpubMeta("Ebook", "Đang cập nhật");
        }
        String normalized = classpathLocation.startsWith("classpath:") || classpathLocation.startsWith("file:")
                ? classpathLocation
                : "classpath:" + classpathLocation;
        Resource resource = resourceLoader.getResource(normalized);
        if (resource.exists()) {
            try {
                EpubPackage epubPackage = readEpubPackage(resource);
                String title = cleanInlineText(epubPackage.title());
                String authors = cleanInlineText(epubPackage.authors());
                if (!title.isBlank()) {
                    return new EpubMeta(title, authors.isBlank() ? "\u0110ang c\u1eadp nh\u1eadt" : authors);
                }
            } catch (Exception ignored) {
                // fallback to file name below
            }
        }
        return new EpubMeta(titleFromFileName(classpathLocation), authorFromFileName(classpathLocation));
    }

    private EpubPackage readEpubPackage(Resource resource) throws IOException {
        for (Charset charset : List.of(StandardCharsets.UTF_8, StandardCharsets.ISO_8859_1)) {
            try {
                EpubPackage pkg = readEpubPackageWithCharset(resource, charset);
                if (!pkg.spineHtmlPaths().isEmpty() || !cleanInlineText(pkg.title()).isBlank()) return pkg;
            } catch (Exception ignored) {
                // thử charset tiếp theo
            }
        }
        return new EpubPackage("", "", List.of());
    }

    private EpubPackage readEpubPackageWithCharset(Resource resource, Charset charset) throws IOException {
        try (InputStream inputStream = resource.getInputStream();
             ZipInputStream zip = new ZipInputStream(inputStream, charset)) {
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null) {
                String name = entry.getName().toLowerCase(Locale.ROOT);
                if (entry.isDirectory() || !name.endsWith(".opf")) continue;
                String opfPath = normalizePath(entry.getName());
                String opf = decodeTextBytes(readCurrentZipEntry(zip));
                return parseOpf(opfPath, opf);
            }
        }
        return new EpubPackage("", "", List.of());
    }

    private EpubPackage parseOpf(String opfPath, String opf) {
        String title = findDcTag(opf, "title");
        String authors = findDcTag(opf, "creator");

        Map<String, String> manifest = new HashMap<>();
        Matcher itemMatcher = Pattern.compile("(?is)<item\\s+([^>]+)>").matcher(opf);
        while (itemMatcher.find()) {
            Map<String, String> attrs = parseAttributes(itemMatcher.group(1));
            String id = attrs.get("id");
            String href = attrs.get("href");
            if (id != null && href != null) {
                manifest.put(id, href);
            }
        }

        String base = "";
        int slash = opfPath.lastIndexOf('/');
        if (slash >= 0) base = opfPath.substring(0, slash + 1);

        List<String> spine = new ArrayList<>();
        Matcher itemRefMatcher = Pattern.compile("(?is)<itemref\\s+([^>]+)>").matcher(opf);
        while (itemRefMatcher.find()) {
            Map<String, String> attrs = parseAttributes(itemRefMatcher.group(1));
            String href = manifest.get(attrs.get("idref"));
            if (href != null && !href.isBlank()) {
                spine.add(normalizePath(base + href));
            }
        }

        return new EpubPackage(title, authors, spine);
    }

    private Map<String, String> parseAttributes(String value) {
        Map<String, String> result = new HashMap<>();
        Matcher matcher = Pattern.compile("([A-Za-z_:][-A-Za-z0-9_:.]*)\\s*=\\s*[\"']([^\"']*)[\"']").matcher(value);
        while (matcher.find()) {
            result.put(matcher.group(1), decodeBasicEntities(matcher.group(2)));
        }
        return result;
    }

    private String findDcTag(String opf, String tag) {
        Matcher matcher = Pattern.compile("(?is)<dc:" + tag + "[^>]*>(.*?)</dc:" + tag + ">").matcher(opf);
        if (matcher.find()) {
            return cleanInlineText(decodeBasicEntities(matcher.group(1).replaceAll("(?is)<[^>]+>", " ")));
        }
        return "";
    }

    private String normalizePath(String value) {
        String normalized = value.replace('\\', '/');
        while (normalized.contains("//")) normalized = normalized.replace("//", "/");
        List<String> parts = new ArrayList<>();
        for (String part : normalized.split("/")) {
            if (part.isBlank() || part.equals(".")) continue;
            if (part.equals("..")) {
                if (!parts.isEmpty()) parts.remove(parts.size() - 1);
            } else {
                parts.add(part);
            }
        }
        return String.join("/", parts);
    }

    private String titleFromFileName(String value) {
        String fileName = cleanFileName(value);
        if (fileName.contains(" - ")) fileName = fileName.substring(0, fileName.indexOf(" - "));
        fileName = fileName.replaceAll("[-_]+", " ").replaceAll("\\s+", " ").trim();
        return toTitleCase(fileName.isBlank() ? "Ebook" : fileName);
    }

    private String authorFromFileName(String value) {
        String fileName = cleanFileName(value);
        if (fileName.contains(" - ")) return fileName.substring(fileName.indexOf(" - ") + 3).trim();
        return "\u0110ang c\u1eadp nh\u1eadt";
    }

    private String cleanFileName(String value) {
        String fileName = value == null ? "Ebook" : value;
        int slash = Math.max(fileName.lastIndexOf('/'), fileName.lastIndexOf('\\'));
        if (slash >= 0) fileName = fileName.substring(slash + 1);
        return fileName.replaceFirst("(?i)\\.epub$", "").trim();
    }

    private String toTitleCase(String value) {
        String[] parts = value.split(" ");
        List<String> titled = new ArrayList<>();
        for (String part : parts) {
            if (part.isBlank()) continue;
            if (part.matches("\\d+")) titled.add(part);
            else if (part.length() == 1) titled.add(part.toUpperCase(Locale.ROOT));
            else titled.add(part.substring(0, 1).toUpperCase(Locale.ROOT) + part.substring(1).toLowerCase(Locale.ROOT));
        }
        return String.join(" ", titled);
    }

    private String cleanInlineText(String value) {
        return value == null ? "" : value.replaceAll("\\s+", " ").trim();
    }

    private byte[] readCurrentZipEntry(ZipInputStream zip) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        byte[] buffer = new byte[8192];
        int read;
        while ((read = zip.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
        return out.toByteArray();
    }

    private String cleanHtml(String html) {
        String value = html
                .replaceAll("(?is)<script[^>]*>.*?</script>", " ")
                .replaceAll("(?is)<style[^>]*>.*?</style>", " ")
                .replaceAll("(?is)<head[^>]*>.*?</head>", " ")
                .replaceAll("(?i)<br\\s*/?>", "\n")
                .replaceAll("(?i)</(p|div|h[1-6]|section|article|chapter|li)>", "\n\n")
                .replaceAll("(?is)<[^>]+>", " ");

        value = decodeBasicEntities(value);
        value = Pattern.compile("[ \\t\\x0B\\f\\r]+").matcher(value).replaceAll(" ");
        value = Pattern.compile("\\n[ \\t]+").matcher(value).replaceAll("\n");
        value = Pattern.compile("\\n{3,}").matcher(value).replaceAll("\n\n");
        return value.trim();
    }

    private String decodeTextBytes(byte[] bytes) {
        if (bytes == null || bytes.length == 0) return "";

        String header = new String(bytes, 0, Math.min(bytes.length, 2048), StandardCharsets.ISO_8859_1);
        String charsetName = "UTF-8";

        Matcher xmlEncoding = Pattern.compile("(?i)encoding\\s*=\\s*['\\\"]([^'\\\"]+)['\\\"]").matcher(header);
        if (xmlEncoding.find()) {
            charsetName = xmlEncoding.group(1).trim();
        } else {
            Matcher metaCharset = Pattern.compile("(?i)charset\\s*=\\s*['\\\"]?([^\\s'\\\"/>;]+)").matcher(header);
            if (metaCharset.find()) charsetName = metaCharset.group(1).trim();
        }

        try {
            return new String(bytes, Charset.forName(charsetName));
        } catch (Exception ignored) {
            return new String(bytes, StandardCharsets.UTF_8);
        }
    }

    private String decodeBasicEntities(String value) {
        if (value == null) return "";
        String decoded = value
                .replace("&nbsp;", " ")
                .replace("&#160;", " ")
                .replace("&amp;", "&")
                .replace("&lt;", "<")
                .replace("&gt;", ">")
                .replace("&quot;", "\"")
                .replace("&apos;", "'")
                .replace("&#39;", "'")
                .replace("&ldquo;", "\u201c")
                .replace("&rdquo;", "\u201d")
                .replace("&lsquo;", "\u2018")
                .replace("&rsquo;", "\u2019")
                .replace("&mdash;", "\u2014")
                .replace("&ndash;", "\u2013")
                .replace("&hellip;", "\u2026");

        decoded = decodeNumericEntityPattern(decoded, Pattern.compile("&#(\\d+);"), 10);
        decoded = decodeNumericEntityPattern(decoded, Pattern.compile("&#x([0-9a-fA-F]+);"), 16);
        return decoded;
    }

    private String decodeNumericEntityPattern(String value, Pattern pattern, int radix) {
        Matcher matcher = pattern.matcher(value);
        StringBuffer buffer = new StringBuffer();
        while (matcher.find()) {
            String replacement;
            try {
                int codePoint = Integer.parseInt(matcher.group(1), radix);
                replacement = new String(Character.toChars(codePoint));
            } catch (Exception ignored) {
                replacement = matcher.group(0);
            }
            matcher.appendReplacement(buffer, Matcher.quoteReplacement(replacement));
        }
        matcher.appendTail(buffer);
        return buffer.toString();
    }

    private List<String> splitIntoChunks(String text, int chunkSize) {
        if (text == null || text.isBlank()) return List.of();
        int safeChunkSize = Math.max(1200, chunkSize);
        String[] paragraphs = text.split("\\n{2,}");
        List<String> chunks = new ArrayList<>();
        StringBuilder current = new StringBuilder();

        for (String rawParagraph : paragraphs) {
            String paragraph = rawParagraph.trim();
            if (paragraph.isBlank()) continue;

            if (current.length() > 0 && current.length() + paragraph.length() + 2 > safeChunkSize) {
                chunks.add(current.toString().trim());
                current.setLength(0);
            }

            if (paragraph.length() > safeChunkSize) {
                if (current.length() > 0) {
                    chunks.add(current.toString().trim());
                    current.setLength(0);
                }
                for (int start = 0; start < paragraph.length(); start += safeChunkSize) {
                    int end = Math.min(paragraph.length(), start + safeChunkSize);
                    String part = paragraph.substring(start, end).trim();
                    if (!part.isBlank()) chunks.add(part);
                }
            } else {
                if (current.length() > 0) current.append("\n\n");
                current.append(paragraph);
            }
        }

        if (current.length() > 0) {
            chunks.add(current.toString().trim());
        }

        return chunks;
    }

    private record SessionInfo(Long userId, Long bookVolumeId) {}
    private record VolumeInfo(Long id, Long bookId, int volumeNo, String dbTitle, String contentPath) {}
    private record ContentAvailability(boolean available, int totalChunks, int totalCharacters, int totalVolumes) {}

    private record EpubSection(String name, String text) {}
    private record DiscoveredEpub(String catalogPath, String fileName) {}

    private record EpubMeta(String title, String authors) {}
    private record EpubPackage(String title, String authors, List<String> spineHtmlPaths) {}

    private record VolumeContentMetadata(
            Long bookId,
            Long bookVolumeId,
            int volumeNo,
            String bookTitle,
            String authors,
            String volumeTitle,
            List<String> allBookChunks,
            int startChunkIndex,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            boolean contentAvailable
    ) {}

    public record ReaderMetadata(
            Long bookVolumeId,
            Long bookId,
            int volumeNo,
            String bookTitle,
            String authors,
            String volumeTitle,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            boolean contentAvailable
    ) {}

    public record ChunkPayload(
            Long sessionId,
            Long userId,
            Long bookVolumeId,
            int chunkNo,
            int totalChunks,
            int totalBookChunks,
            int totalVolumes,
            int pagesPerVolume,
            String bookTitle,
            String authors,
            String volumeTitle,
            String content
    ) {}
}