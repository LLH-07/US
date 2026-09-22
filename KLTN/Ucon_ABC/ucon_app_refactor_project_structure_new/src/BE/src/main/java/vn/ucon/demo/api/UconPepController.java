package vn.ucon.demo.api;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;

import vn.ucon.demo.api.socket.PepControlSocketHandler;
import vn.ucon.demo.resource.EbookAdminService;
import vn.ucon.demo.resource.EbookReadService;
import vn.ucon.demo.resource.FamilyService;
import vn.ucon.demo.resource.PurchaseService;
import vn.ucon.demo.resource.SubscriptionService;
import vn.ucon.demo.ucon.ch.ContextHandler;
import vn.ucon.demo.ucon.model.AccessDecision;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.AccessEvaluationAttempt;
import vn.ucon.demo.ucon.model.UsageGrant;

import java.util.LinkedHashMap;
import java.util.Map;

@CrossOrigin(origins = "*")
@RestController
@RequestMapping("/api/pep")
public class UconPepController {

        private final ContextHandler contextHandler;
        private final EbookReadService readService;
        private final SubscriptionService subscriptionService;
        private final PurchaseService purchaseService;
        private final FamilyService familyService;
        private final EbookAdminService ebookAdminService;
        private final PepControlSocketHandler controlSocketHandler;

        public UconPepController(
                ContextHandler contextHandler,
                EbookReadService readService,
                SubscriptionService subscriptionService,
                PurchaseService purchaseService,
                FamilyService familyService,
                EbookAdminService ebookAdminService,
                PepControlSocketHandler controlSocketHandler
        ) {
                this.contextHandler = contextHandler;
                this.readService = readService;
                this.subscriptionService = subscriptionService;
                this.purchaseService = purchaseService;
                this.familyService = familyService;
                this.ebookAdminService = ebookAdminService;
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
                return ResponseEntity.ok(readService.catalog());
        }

        @GetMapping("/read/metadata")
        public ResponseEntity<?> readMetadata(@RequestParam Long volumeId) {
                readService.syncBookTitleFromEpub(volumeId);
                EbookReadService.ReaderMetadata metadata = readService.metadataForVolumePublic(volumeId);
                return ResponseEntity.ok(metadataBody(metadata));
        }

        @GetMapping("/read/history")
        public ResponseEntity<?> readHistory(
                @RequestParam Long userId,
                @RequestParam(defaultValue = "30") int days,
                @RequestParam(defaultValue = "30") int limit
        ) {
                return ResponseEntity.ok(readService.readHistoryForUser(userId, days, limit));
        }

        @GetMapping("/read/preview/chunks")
        public ResponseEntity<?> getPreviewChunk(
                @RequestParam Long userId,
                @RequestParam Long volumeId,
                @RequestParam int chunkNo
        ) {
                EbookReadService.ChunkPayload chunk = readService.fetchPreviewChunk(userId, volumeId, chunkNo);
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

                readService.syncBookTitleFromEpub(volumeId);
                EbookReadService.ReaderMetadata metadata =
                        readService.metadataForVolumePublic(volumeId);

                String requestedAccessPath;
                try {
                        requestedAccessPath = normalizeRequestedAccessPath(accessPath);
                } catch (IllegalArgumentException ex) {
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "INVALID_ACCESS_PATH",
                                "reason", ex.getMessage()
                        ));
                }

                boolean shouldBindControlChannel =
                        connectionId != null && !connectionId.isBlank();

                if (shouldBindControlChannel
                        && !controlSocketHandler.isBindable(connectionId, userId)) {
                        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Map.of(
                                "message", "CONTROL_CHANNEL_NOT_AVAILABLE",
                                "reason", "Closed or mismatched WebSocket connection"
                        ));
                }

                AccessRequest request = new AccessRequest(
                        userId,
                        "BOOK_VOLUME",
                        volumeId,
                        "READ",
                        requestedAccessPath,
                        "PRE",
                        null
                );

                AccessDecision decision = contextHandler.handleTryAccess(request);

                if (!decision.permit()) {
                        Map<String, Object> body = metadataBody(metadata);
                        body.put("message", "DENY");
                        body.put("reason", decision.reasonCode());
                        body.put("requestedAccessPath", requestedAccessPath);
                        body.put("attempts", decision.attempts().stream()
                                .map(this::attemptBody)
                                .toList());
                        body.put("pepFlow", "PEP_CH_CANDIDATE_EVALUATION");

                        return ResponseEntity.status(HttpStatus.FORBIDDEN).body(body);
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

                String selectedAccessPath = decision.selectedAccessPath() == null
                        ? requestedAccessPath
                        : decision.selectedAccessPath();

                Map<String, Object> body = metadataBody(metadata);
                body.put("message", "PERMIT");
                body.put("readMode", "FULL");
                body.put("previewLimit", null);
                body.put("sessionId", decision.usageSessionId());
                body.put("usageSessionId", decision.usageSessionId());
                body.put("userId", userId);
                body.put("requestedAccessPath", requestedAccessPath);
                body.put("selectedAccessPath", selectedAccessPath);
                body.put("accessPath", selectedAccessPath);
                body.put("attempts", decision.attempts().stream()
                        .map(this::attemptBody)
                        .toList());
                body.put("controlBound", controlBound);
                body.put("pepFlow", "PEP_CH_RESOURCE");

                return ResponseEntity.ok(body);
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

                EbookReadService.ChunkPayload chunk = readService.fetchChunk(
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
                return ResponseEntity.ok(subscriptionService.subscriptionStatusForUser(userId));
        }

        @PostMapping("/subscriptions/register")
        public ResponseEntity<?> registerSubscription(
                @RequestParam Long userId,
                @RequestParam Long planId,
                @RequestParam(required = false) Integer demoSeconds,
                @RequestParam(required = false) Integer demoMinutes,
                @RequestParam(required = false) Integer demoDays
        ) {
                return ResponseEntity.ok(subscriptionService.registerSubscription(userId, planId, demoSeconds, demoMinutes, demoDays));
        }

        @PostMapping("/subscriptions/cancel")
        public ResponseEntity<?> cancelSubscription(@RequestParam Long userId) {
                return ResponseEntity.ok(subscriptionService.cancelSubscription(userId));
        }

        @GetMapping("/purchases/library")
        public ResponseEntity<?> purchaseLibrary(@RequestParam Long userId) {
                return ResponseEntity.ok(purchaseService.purchaseLibraryForUser(userId));
        }

        @PostMapping("/purchases/book")
        public ResponseEntity<?> purchaseBook(
                @RequestParam Long userId,
                @RequestParam Long bookId
        ) {
                return ResponseEntity.ok(purchaseService.purchaseBook(userId, bookId));
        }

        @PostMapping("/purchases/volume")
        public ResponseEntity<?> purchaseVolume(
                @RequestParam Long userId,
                @RequestParam Long volumeId
        ) {
                return ResponseEntity.ok(purchaseService.purchaseVolume(userId, volumeId));
        }

        @GetMapping("/families")
        public ResponseEntity<?> familyData(@RequestParam Long userId) {
                return ResponseEntity.ok(familyService.familyDataForUser(userId));
        }

        @PostMapping("/families/{groupId}/members")
        public ResponseEntity<?> addFamilyMember(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam(required = false) Long memberUserId,
                @RequestParam(required = false) String memberEmail
        ) {
                return ResponseEntity.ok(familyService.addFamilyMember(groupId, ownerUserId, memberUserId, memberEmail));
        }

        @PostMapping("/families/{groupId}/members/{memberUserId}/remove")
        public ResponseEntity<?> removeFamilyMember(
                @PathVariable Long groupId,
                @PathVariable Long memberUserId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(familyService.removeFamilyMember(groupId, ownerUserId, memberUserId));
        }

        @PostMapping("/families/{groupId}/leave")
        public ResponseEntity<?> leaveFamilyGroup(
                @PathVariable Long groupId,
                @RequestParam Long userId
        ) {
                return ResponseEntity.ok(familyService.leaveFamilyGroup(groupId, userId));
        }

        @PostMapping("/families/{groupId}/dissolve")
        public ResponseEntity<?> dissolveFamilyGroup(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(familyService.dissolveFamilyGroup(groupId, ownerUserId));
        }

        @PostMapping("/families/{groupId}/shared-books")
        public ResponseEntity<?> shareFamilyBook(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam Long bookId
        ) {
                return ResponseEntity.ok(familyService.shareFamilyBook(groupId, ownerUserId, bookId));
        }

        @PostMapping("/families/{groupId}/shared-books/{bookId}/remove")
        public ResponseEntity<?> removeFamilySharedBook(
                @PathVariable Long groupId,
                @PathVariable Long bookId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(familyService.removeFamilySharedBook(groupId, bookId, ownerUserId));
        }

        @PostMapping("/families/{groupId}/shared-volumes")
        public ResponseEntity<?> shareFamilyVolume(
                @PathVariable Long groupId,
                @RequestParam Long ownerUserId,
                @RequestParam Long volumeId
        ) {
                return ResponseEntity.ok(familyService.shareFamilyVolume(groupId, ownerUserId, volumeId));
        }

        @PostMapping("/families/{groupId}/shared-volumes/{shareId}/remove")
        public ResponseEntity<?> removeFamilySharedVolume(
                @PathVariable Long groupId,
                @PathVariable Long shareId,
                @RequestParam Long ownerUserId
        ) {
                return ResponseEntity.ok(familyService.removeFamilySharedVolume(groupId, shareId, ownerUserId));
        }

        @PostMapping("/admin/books/sync-ebooks")
        public ResponseEntity<?> syncEbooksFromAdmin(@RequestParam Long adminUserId) {
                return ResponseEntity.ok(ebookAdminService.syncEbooksFromAdmin(adminUserId));
        }

        private Map<String, Object> metadataBody(EbookReadService.ReaderMetadata metadata) {
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

        private Map<String, Object> attemptBody(
                AccessEvaluationAttempt attempt
        ) {
                Map<String, Object> body = new LinkedHashMap<>();

                body.put("accessPath", attempt.accessPath());
                body.put("result", attempt.result());
                body.put("reasonCode", attempt.reasonCode());

                if (attempt.rawDecision() != null) {
                        body.put("rawDecision", attempt.rawDecision());
                }

                if (attempt.policyRef() != null) {
                        body.put("policyRef", attempt.policyRef());
                }

                if (attempt.message() != null) {
                        body.put("message", attempt.message());
                }

                body.put(
                        "requiredObligations",
                        attempt.requiredObligations()
                );

                return body;
        }

        private String normalizeRequestedAccessPath(String accessPath) {
                if (accessPath == null || accessPath.isBlank()) {
                        return "AUTO";
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
                        case "AUTO", "FREE", "RENTAL", "PAID", "FAMILY_SHARED" -> normalized;
                        default -> throw new IllegalArgumentException(
                                "Unsupported accessPath: " + accessPath
                        );
                };
        }
}