package vn.ucon.demo.api;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import vn.ucon.demo.ucon.obligation.FulfillObligationRequest;
import vn.ucon.demo.ucon.obligation.FulfillObligationResult;
import vn.ucon.demo.ucon.obligation.ObligationManager;

@CrossOrigin(origins = "*")
@RestController
@RequestMapping("/api/ucon/obligations")
public class ObligationController {

    private final ObligationManager obligationFulfillmentService;

    public ObligationController(
            ObligationManager obligationFulfillmentService
    ) {
        this.obligationFulfillmentService = obligationFulfillmentService;
    }

    @PostMapping("/{obligationId}/fulfill")
    public ResponseEntity<FulfillObligationResult> fulfill(
            @PathVariable String obligationId,
            @RequestBody FulfillObligationRequest request
    ) {
        return ResponseEntity.ok(
                obligationFulfillmentService.fulfill(
                        obligationId,
                        request
                )
        );
    }
}