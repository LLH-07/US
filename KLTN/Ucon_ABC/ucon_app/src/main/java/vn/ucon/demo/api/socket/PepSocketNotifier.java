package vn.ucon.demo.api.socket;

import org.springframework.stereotype.Component;
import org.springframework.transaction.event.TransactionPhase;
import org.springframework.transaction.event.TransactionalEventListener;

import vn.ucon.demo.ucon.model.UsageSessionEvent;

@Component
public class PepSocketNotifier {

    private final PepControlSocketHandler socketHandler;

    public PepSocketNotifier(PepControlSocketHandler socketHandler) {
        this.socketHandler = socketHandler;
    }

    @TransactionalEventListener(
            phase = TransactionPhase.AFTER_COMMIT,
            fallbackExecution = true
    )
    public void onUsageSessionEvent(UsageSessionEvent event) {
        switch (event.type()) {
            case REVOKED -> socketHandler.notifyRevoked(
                    event.usageSessionId(),
                    event.reasonCode()
            );

            case ENDED -> socketHandler.notifyEnded(
                    event.usageSessionId(),
                    event.reasonCode()
            );
        }
    }
}