package vn.ucon.demo.ucon.model;

import java.time.OffsetDateTime;
import java.time.ZoneId;

public record SessionControlMessage(
        String type,
        String connectionId,
        Long usageSessionId,
        String reason,
        String message,
        OffsetDateTime at
) {
    private static final ZoneId APP_ZONE = ZoneId.of("Asia/Ho_Chi_Minh");

    private static OffsetDateTime now() {
        return OffsetDateTime.now(APP_ZONE);
    }

    public static SessionControlMessage connected(String connectionId) {
        return new SessionControlMessage(
                "CONTROL_CONNECTED",
                connectionId,
                null,
                null,
                "Control channel connected",
                now()
        );
    }

    public static SessionControlMessage usageBound(
            String connectionId,
            Long usageSessionId
    ) {
        return new SessionControlMessage(
                "USAGE_BOUND",
                connectionId,
                usageSessionId,
                null,
                "Usage session bound to control channel",
                now()
        );
    }

    public static SessionControlMessage revoked(
            String connectionId,
            Long usageSessionId,
            String reason
    ) {
        return new SessionControlMessage(
                "SESSION_REVOKED",
                connectionId,
                usageSessionId,
                reason,
                "Usage session revoked",
                now()
        );
    }

    public static SessionControlMessage ended(
            String connectionId,
            Long usageSessionId,
            String reason
    ) {
        return new SessionControlMessage(
                "SESSION_ENDED",
                connectionId,
                usageSessionId,
                reason,
                "Usage session ended",
                now()
        );
    }

    public static SessionControlMessage pong(String connectionId) {
        return new SessionControlMessage(
                "PONG",
                connectionId,
                null,
                null,
                "pong",
                now()
        );
    }

    public static SessionControlMessage error(String message) {
        return new SessionControlMessage(
                "ERROR",
                null,
                null,
                null,
                message,
                now()
        );
    }
}