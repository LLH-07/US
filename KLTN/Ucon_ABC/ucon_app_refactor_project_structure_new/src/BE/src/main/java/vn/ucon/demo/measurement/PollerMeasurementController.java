package vn.ucon.demo.measurement;

import org.springframework.context.annotation.Profile;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import vn.ucon.demo.ucon.event.UconTriggerEventPoller;
import vn.ucon.demo.ucon.event.UconTriggerEventPoller.PollerMetrics;

@RestController
@RequestMapping("/api/measurement/poller")
@Profile("measurement")
public class PollerMeasurementController {

    private final UconTriggerEventPoller poller;

    public PollerMeasurementController(
            UconTriggerEventPoller poller
    ) {
        this.poller = poller;
    }

    @PostMapping("/reset")
    public void reset() {
        poller.resetMetrics();
    }

    @GetMapping("/snapshot")
    public PollerMetrics snapshot() {
        return poller.snapshotMetrics();
    }
}