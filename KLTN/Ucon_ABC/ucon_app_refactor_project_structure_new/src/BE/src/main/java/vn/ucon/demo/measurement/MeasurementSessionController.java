package vn.ucon.demo.measurement;

import org.springframework.context.annotation.Profile;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import vn.ucon.demo.ucon.ch.ContextHandler;
import vn.ucon.demo.ucon.model.AccessDecision;
import vn.ucon.demo.ucon.model.AccessRequest;

@RestController
@RequestMapping("/api/measurement/sessions")
@Profile("measurement")
public class MeasurementSessionController {

    private final ContextHandler contextHandler;

    public MeasurementSessionController(
            ContextHandler contextHandler
    ) {
        this.contextHandler = contextHandler;
    }

    @PostMapping("/try-rental")
    public ResponseEntity<AccessDecision> tryRentalAccess(
            @RequestParam Long userId,
            @RequestParam Long volumeId
    ) {
        AccessRequest request = new AccessRequest(
                userId,
                "BOOK_VOLUME",
                volumeId,
                "READ",
                "RENTAL",
                "PRE",
                null
        );

        AccessDecision decision =
                contextHandler.handleTryAccess(request);

        if (!decision.permit()) {
            return ResponseEntity
                    .status(HttpStatus.FORBIDDEN)
                    .body(decision);
        }

        return ResponseEntity.ok(decision);
    }
}