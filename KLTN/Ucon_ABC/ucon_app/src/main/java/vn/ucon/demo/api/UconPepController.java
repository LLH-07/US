package vn.ucon.demo.api;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;

import vn.ucon.demo.api.socket.PepControlSocketHandler;
import vn.ucon.demo.resource.EbookResourceService;
import vn.ucon.demo.ucon.ch.ContextHandler;
import vn.ucon.demo.ucon.model.AccessDecision;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.UsageGrant;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

@CrossOrigin(origins = "*")
@RestController
@RequestMapping("/api/pep")
public class UconPepController {

        private final ContextHandler contextHandler;
        private final EbookResourceService resourceService;
        private final PepControlSocketHandler controlSocketHandler;

        public UconPepController(
                ContextHandler contextHandler,
                EbookResourceService resourceService,
                PepControlSocketHandler controlSocketHandler
        ) {
                this.contextHandler = contextHandler;
                this.resourceService = resourceService;
                this.controlSocketHandler = controlSocketHandler;
        }

        @ExceptionHandler(ResponseStatusException.class)
        public ResponseEntity<?> handleResponseStatusException(ResponseStatusException ex) {
                String reason = ex.getReason() == null ? "REQUEST_FAILED" : ex.getReason();
                return ResponseEntity.status(ex.getStatusCode()).body(Map.of(
                        "message", "ERROR",
                        "reason", reason
                ));
        }

        @GetMapping("/read/catalog")
        public ResponseEntity<?> readCatalog() {
                return ResponseEntity.ok(resourceService.catalog());
        }

        @GetMapping("/read/metadata")
        public ResponseEntity<?> readMetadata(@RequestParam Long volumeId) {
                resourceService.syncBookTitleFromEpub(volumeId);
                EbookResourceService.ReaderMetadata metadata = resourceService.metadataForVolumePublic(volumeId);
                return ResponseEntity.ok(metadataBody(metadata));
        }

        @GetMapping("/read/history")
        public ResponseEntity<?> readHistory(
                @RequestParam Long userId,
                @RequestParam(defaultValue = "30") int days,
                @RequestParam(defaultValue = "30") int limit
        ) {
                return ResponseEntity.ok(resourceService.readHistoryForUser(userId, days, limit));
        }

        @GetMapping("/read/preview/chunks")
        public ResponseEntity<?> getPreviewChunk(
                @RequestParam Long userId,
                @RequestParam Long volumeId,
                @RequestParam int chunkNo
        ) {
                EbookResourceService.ChunkPayload chunk = resourceService.fetchPreviewChunk(userId, volumeId, chunkNo);
                Map<String, Object> body = new LinkedHashMap<>();
                body.put("message", "PERMIT");
                body.put("sessionId", null);
                body.put("userId", chunk.userId());
                body.put("bookVolumeId", chunk.bookVolumeId());
                body.put("chunkNo", chunk.chunkNo());
                body.put("totalChunks", Math.min(5, chunk.totalChunks()));
                body.put("totalBookChunks", chunk.totalBookChunks());
                body.put("totalVolumes", chunk.totalVolumes());
                body.put("pagesPerVolume", chunk.pagesPerVolume());
                body.put("bookTitle", chunk.bookTitle());
                body.put("authors", chunk.authors());
                body.put("volumeTitle", chunk.volumeTitle());
                body.put("readMode", "PREVIEW");
                body.put("previewLimit", 5);
                body.put("content", chunk.content());
                body.put("pepFlow", "STATIC_PREVIEW_NO_UCON");
                return ResponseEntity.ok(body);
        }

        @PostMapping("/read/try")
        public ResponseEntity<?> startRead(
                @RequestParam Long userId,
                @RequestParam Long volumeId,
                @RequestParam(defaultValue = "FULL") String mode,
                @RequestParam(required = false) String accessPath,
                @RequestHeader(value = "X-Control-Connection-Id", required = false) String connectionId
        ) {
                if (!"FULL".equalsIgnoreCase(mode)) {
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "PREVIEW_USES_STATIC_ENDPOINT",
                                "reason", "PREVIEW_DOES_NOT_CREATE_UCON_SESSION"
                        ));
                }

                resourceService.syncBookTitleFromEpub(volumeId);
                EbookResourceService.ReaderMetadata metadata = resourceService.metadataForVolumePublic(volumeId);

                String rawAccessPath = accessPath == null || accessPath.isBlank()
                        ? resourceService.accessModelOverrideForVolume(volumeId)
                        : accessPath;

                String normalizedBaseAccessPath;
                try {
                        normalizedBaseAccessPath = normalizeAccessPath(rawAccessPath);
                } catch (IllegalArgumentException ex) {
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "INVALID_ACCESS_PATH",
                                "reason", ex.getMessage()
                        ));
                }

                boolean shouldBindControlChannel = connectionId != null && !connectionId.isBlank();
                if (shouldBindControlChannel && !controlSocketHandler.isBindable(connectionId, userId)) {
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "CONTROL_CHANNEL_NOT_AVAILABLE",
                                "reason", "Closed or mismatched WebSocket connection"
                        ));
                }

                List<Map<String, Object>> deniedAttempts = new ArrayList<>();

                for (String candidateAccessPath : candidateAccessPaths(normalizedBaseAccessPath)) {
                        AccessRequest request = new AccessRequest(
                                userId,
                                "BOOK_VOLUME",
                                volumeId,
                                "READ",
                                candidateAccessPath,
                                "PRE",
                                null
                        );

                        AccessDecision decision;
                        try {
                                decision = contextHandler.handleTryAccess(request);
                        } catch (IllegalStateException ex) {
                                if (!isNoEnabledPolicyError(ex)) {
                                        throw ex;
                                }

                                Map<String, Object> attempt = new LinkedHashMap<>();
                                attempt.put("accessPath", candidateAccessPath);
                                attempt.put("reason", "NO_ENABLED_POLICY");
                                attempt.put("detail", ex.getMessage());
                                deniedAttempts.add(attempt);
                                continue;
                        }

                        if (!decision.permit()) {
                                Map<String, Object> attempt = new LinkedHashMap<>();
                                attempt.put("accessPath", candidateAccessPath);
                                attempt.put("reason", decision.reasonCode());
                                deniedAttempts.add(attempt);
                                continue;
                        }

                        boolean controlBound = false;
                        if (shouldBindControlChannel) {
                                controlBound = controlSocketHandler.bindUsage(
                                        connectionId,
                                        decision.usageSessionId(),
                                        userId
                                );

                                if (!controlBound) {
                                        return ResponseEntity.status(HttpStatus.CONFLICT).body(Map.of(
                                                "message", "CONTROL_CHANNEL_LOST",
                                                "reason", "WebSocket connection was closed before binding",
                                                "usageSessionId", decision.usageSessionId(),
                                                "sessionId", decision.usageSessionId()
                                        ));
                                }
                        }

                        Map<String, Object> body = metadataBody(metadata);
                        body.put("message", "PERMIT");
                        body.put("readMode", "FULL");
                        body.put("previewLimit", null);
                        body.put("sessionId", decision.usageSessionId());
                        body.put("usageSessionId", decision.usageSessionId());
                        body.put("userId", userId);
                        body.put("accessModel", normalizedBaseAccessPath);
                        body.put("accessPath", candidateAccessPath);
                        body.put("attemptedAccessPaths", candidateAccessPaths(normalizedBaseAccessPath));
                        body.put("controlBound", controlBound);
                        body.put("pepFlow", "PEP_CH_RESOURCE");
                        return ResponseEntity.ok(body);
                }

                Map<String, Object> body = new LinkedHashMap<>();
                body.put("message", "DENY");
                body.put("reason", "ALL_ACCESS_PATHS_DENIED");
                body.put("accessModel", normalizedBaseAccessPath);
                body.put("attempts", deniedAttempts);
                return ResponseEntity.status(HttpStatus.FORBIDDEN).body(body);
        }

        @GetMapping("/read/sessions/{sessionId}/chunks/{chunkNo}")
        public ResponseEntity<?> getChunk(
                @PathVariable Long sessionId,
                @PathVariable int chunkNo
        ) {
                UsageGrant grant = contextHandler.authorizeUsage(sessionId);

                if (!grant.permit()) {
                        return ResponseEntity.status(HttpStatus.FORBIDDEN).body(Map.of(
                                "message", "DENY_OR_REVOKED",
                                "reason", grant.reasonCode()
                        ));
                }

                EbookResourceService.ChunkPayload chunk = resourceService.fetchChunk(
                        sessionId,
                        grant.resourceId(),
                        chunkNo
                );

                Map<String, Object> body = new LinkedHashMap<>();
                body.put("message", "PERMIT");
                body.put("sessionId", sessionId);
                body.put("usageSessionId", sessionId);
                body.put("userId", chunk.userId());
                body.put("bookVolumeId", chunk.bookVolumeId());
                body.put("chunkNo", chunk.chunkNo());
                body.put("totalChunks", chunk.totalChunks());
                body.put("totalBookChunks", chunk.totalBookChunks());
                body.put("totalVolumes", chunk.totalVolumes());
                body.put("pagesPerVolume", chunk.pagesPerVolume());
                body.put("bookTitle", chunk.bookTitle());
                body.put("authors", chunk.authors());
                body.put("volumeTitle", chunk.volumeTitle());
                body.put("content", chunk.content());
                return ResponseEntity.ok(body);
        }

        @PostMapping("/read/sessions/{sessionId}/end")
        public ResponseEntity<?> endRead(@PathVariable Long sessionId) {
                AccessDecision decision = contextHandler.handleEndAccess(sessionId);

                if (!decision.permit()) {
                        String reason = decision.reasonCode() == null ? "END_ACCESS_FAILED" : decision.reasonCode();
                        if ("SESSION_NOT_ACTIVE".equals(reason) || "SESSION_NOT_STARTABLE".equals(reason)) {
                                return ResponseEntity.ok(Map.of(
                                        "message", "SESSION_ALREADY_CLOSED_OR_NOT_ACTIVE",
                                        "reason", reason,
                                        "sessionId", sessionId,
                                        "usageSessionId", sessionId,
                                        "pepFlow", "PEP_CH_END_IDEMPOTENT"
                                ));
                        }
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "END_ACCESS_FAILED",
                                "reason", reason
                        ));
                }

                return ResponseEntity.ok(Map.of(
                        "message", "SESSION_ENDED",
                        "sessionId", sessionId,
                        "usageSessionId", sessionId,
                        "pepFlow", "PEP_CH_RESOURCE"
                ));
        }

        @GetMapping("/subscriptions/status")
        public ResponseEntity<?> subscriptionStatus(@RequestParam Long userId) {
                return ResponseEntity.ok(resourceService.subscriptionStatusForUser(userId));
        }

        @PostMapping("/subscriptions/register")
        public ResponseEntity<?> registerSubscription(
                @RequestParam Long userId,
                @RequestParam Long planId,
                @RequestParam(required = false) Integer demoSeconds,
                @RequestParam(required = false) Integer demoMinutes,
                @RequestParam(required = false) Integer demoDays
        ) {
                return ResponseEntity.ok(resourceService.registerSubscription(userId, planId, demoSeconds, demoMinutes, demoDays));
        }

        @PostMapping("/subscriptions/cancel")
        public ResponseEntity<?> cancelSubscription(@RequestParam Long userId) {
                return ResponseEntity.ok(resourceService.cancelSubscription(userId));
        }

        @GetMapping("/purchases/library")
        public ResponseEntity<?> purchaseLibrary(@RequestParam Long userId) {
                return ResponseEntity.ok(resourceService.purchaseLibraryForUser(userId));
        }

        @PostMapping("/purchases/book")
        public ResponseEntity<?> purchaseBook(
                @RequestParam Long userId,
                @RequestParam Long bookId
        ) {
                return ResponseEntity.ok(resourceService.purchaseBook(userId, bookId));
        }

        @PostMapping("/purchases/volume")
        public ResponseEntity<?> purchaseVolume(
                @RequestParam Long userId,
                @RequestParam Long volumeId
        ) {
                return ResponseEntity.ok(resourceService.purchaseVolume(userId, volumeId));
        }

        @GetMapping("/families")
        public ResponseEntity<?> familyData(@RequestParam Long userId) {
                return ResponseEntity.ok(resourceService.familyDataForUser(userId));
        }

        @PostMapping("/families/{groupId}/members")
        public ResponseEntity<?> addFamilyMember(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam(required = false) Long memberUserId,
                @RequestParam(required = false) String memberEmail
        ) {
                return ResponseEntity.ok(resourceService.addFamilyMember(groupId, ownerUserId, memberUserId, memberEmail));
        }

        @PostMapping("/families/{groupId}/members/{memberUserId}/remove")
        public ResponseEntity<?> removeFamilyMember(
                @PathVariable Long groupId,
                @PathVariable Long memberUserId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(resourceService.removeFamilyMember(groupId, ownerUserId, memberUserId));
        }

        @PostMapping("/families/{groupId}/leave")
        public ResponseEntity<?> leaveFamilyGroup(
                @PathVariable Long groupId,
                @RequestParam Long userId
        ) {
                return ResponseEntity.ok(resourceService.leaveFamilyGroup(groupId, userId));
        }

        @PostMapping("/families/{groupId}/dissolve")
        public ResponseEntity<?> dissolveFamilyGroup(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(resourceService.dissolveFamilyGroup(groupId, ownerUserId));
        }

        @PostMapping("/families/{groupId}/shared-books")
        public ResponseEntity<?> shareFamilyBook(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam Long bookId
        ) {
                return ResponseEntity.ok(resourceService.shareFamilyBook(groupId, ownerUserId, bookId));
        }

        @PostMapping("/families/{groupId}/shared-books/{bookId}/remove")
        public ResponseEntity<?> removeFamilySharedBook(
                @PathVariable Long groupId,
                @PathVariable Long bookId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(resourceService.removeFamilySharedBook(groupId, bookId, ownerUserId));
        }

        @PostMapping("/families/{groupId}/shared-volumes")
        public ResponseEntity<?> shareFamilyVolume(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam Long volumeId
        ) {
                return ResponseEntity.ok(resourceService.shareFamilyVolume(groupId, ownerUserId, volumeId));
        }

        @PostMapping("/families/{groupId}/shared-volumes/{shareId}/remove")
        public ResponseEntity<?> removeFamilySharedVolume(
                @PathVariable Long groupId,
                @PathVariable Long shareId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(resourceService.removeFamilySharedVolume(groupId, shareId, ownerUserId));
        }

        @PostMapping("/admin/books/sync-ebooks")
        public ResponseEntity<?> syncEbooksFromAdmin(@RequestParam Long adminUserId) {
                return ResponseEntity.ok(resourceService.syncEbooksFromAdmin(adminUserId));
        }

        private Map<String, Object> metadataBody(EbookResourceService.ReaderMetadata metadata) {
                Map<String, Object> body = new LinkedHashMap<>();
                body.put("bookVolumeId", metadata.bookVolumeId());
                body.put("bookId", metadata.bookId());
                body.put("volumeNo", metadata.volumeNo());
                body.put("bookTitle", metadata.bookTitle());
                body.put("authors", metadata.authors());
                body.put("volumeTitle", metadata.volumeTitle());
                body.put("totalChunks", metadata.totalChunks());
                body.put("totalBookChunks", metadata.totalBookChunks());
                body.put("totalVolumes", metadata.totalVolumes());
                body.put("pagesPerVolume", metadata.pagesPerVolume());
                body.put("contentAvailable", metadata.contentAvailable());
                return body;
        }

        private List<String> candidateAccessPaths(String normalizedAccessPath) {
                if ("PAID".equals(normalizedAccessPath)) {
                        return List.of("PAID", "FAMILY_SHARED");
                }
                return List.of(normalizedAccessPath);
        }

        private boolean isNoEnabledPolicyError(IllegalStateException ex) {
                String message = ex.getMessage();
                return message != null && message.startsWith("No enabled policy found");
        }

        private String normalizeAccessPath(String accessPath) {
                if (accessPath == null || accessPath.isBlank()) {
                        return "RENTAL";
                }

                String normalized = accessPath.trim()
                        .toUpperCase()
                        .replace("-", "_");

                if ("FAMILY_SHARE".equals(normalized)) {
                        return "FAMILY_SHARED";
                }

                if ("PURCHASE".equals(normalized) || "PURCHASED".equals(normalized)) {
                        return "PAID";
                }

                return switch (normalized) {
                        case "FREE", "RENTAL", "PAID", "FAMILY_SHARED" -> normalized;
                        default -> throw new IllegalArgumentException(
                                "Unsupported accessPath: " + accessPath
                        );
                };
        }
}
