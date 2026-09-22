package vn.ucon.demo.api.socket;

import java.io.IOException;
import java.net.URI;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import vn.ucon.demo.ucon.model.SessionControlMessage;

@Component
public class PepControlSocketHandler extends TextWebSocketHandler {

    private final ObjectMapper objectMapper;

    private final Map<String, WebSocketSession> sessionsByConnectionId =
            new ConcurrentHashMap<>();

    private final Map<String, Long> usageSessionByConnectionId =
            new ConcurrentHashMap<>();

    private final Map<Long, String> connectionIdByUsageSessionId =
            new ConcurrentHashMap<>();

    public PepControlSocketHandler(ObjectMapper objectMapper) {
        this.objectMapper = objectMapper;
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession socketSession) throws Exception {
        String connectionId = UUID.randomUUID().toString();
        Long userId = extractUserId(socketSession.getUri());

        socketSession.getAttributes().put("connectionId", connectionId);
        socketSession.getAttributes().put("userId", userId);

        sessionsByConnectionId.put(connectionId, socketSession);

        send(socketSession, SessionControlMessage.connected(connectionId));
    }

    @Override
    protected void handleTextMessage(
            WebSocketSession socketSession,
            TextMessage message
    ) throws Exception {
        String payload = message.getPayload();

        if ("PING".equalsIgnoreCase(payload)) {
            String connectionId = (String) socketSession
                    .getAttributes()
                    .get("connectionId");

            send(socketSession, SessionControlMessage.pong(connectionId));
            return;
        }

        send(socketSession, SessionControlMessage.error(
                "Unsupported socket message. Current demo supports only PING."
        ));
    }

    @Override
    public void afterConnectionClosed(
            WebSocketSession socketSession,
            CloseStatus status
    ) {
        remove(socketSession);
    }

    @Override
    public void handleTransportError(
            WebSocketSession socketSession,
            Throwable exception
    ) throws Exception {
        remove(socketSession);

        if (socketSession.isOpen()) {
            socketSession.close(CloseStatus.SERVER_ERROR);
        }
    }

    public boolean isBindable(String connectionId, Long userId) {
        WebSocketSession socketSession = sessionsByConnectionId.get(connectionId);

        if (socketSession == null || !socketSession.isOpen()) {
            return false;
        }

        Long socketUserId = (Long) socketSession.getAttributes().get("userId");

        return socketUserId != null && socketUserId.equals(userId);
    }

    public boolean bindUsage(
            String connectionId,
            Long usageSessionId,
            Long userId
    ) {
        WebSocketSession socketSession = sessionsByConnectionId.get(connectionId);

        if (socketSession == null || !socketSession.isOpen()) {
            return false;
        }

        Long socketUserId = (Long) socketSession.getAttributes().get("userId");

        if (socketUserId == null || !socketUserId.equals(userId)) {
            return false;
        }

        Long previousUsageSessionId = usageSessionByConnectionId.put(
                connectionId,
                usageSessionId
        );

        if (previousUsageSessionId != null) {
            connectionIdByUsageSessionId.remove(previousUsageSessionId);
        }

        String previousConnectionId = connectionIdByUsageSessionId.put(
                usageSessionId,
                connectionId
        );

        if (previousConnectionId != null && !previousConnectionId.equals(connectionId)) {
            usageSessionByConnectionId.remove(previousConnectionId);
        }

        try {
            send(socketSession, SessionControlMessage.usageBound(
                    connectionId,
                    usageSessionId
            ));
            return true;
        } catch (IOException e) {
            remove(socketSession);
            return false;
        }
    }

    public void notifyRevoked(Long usageSessionId, String reasonCode) {
        notifyAndUnbind(
                usageSessionId,
                "SESSION_REVOKED",
                reasonCode
        );
    }

    public void notifyEnded(Long usageSessionId, String reasonCode) {
        notifyAndUnbind(
                usageSessionId,
                "SESSION_ENDED",
                reasonCode
        );
    }

    private void notifyAndUnbind(
            Long usageSessionId,
            String type,
            String reasonCode
    ) {
        String connectionId = connectionIdByUsageSessionId.remove(usageSessionId);

        if (connectionId == null) {
            return;
        }

        usageSessionByConnectionId.remove(connectionId);

        WebSocketSession socketSession = sessionsByConnectionId.get(connectionId);

        if (socketSession == null || !socketSession.isOpen()) {
            return;
        }

        SessionControlMessage message = switch (type) {
            case "SESSION_REVOKED" -> SessionControlMessage.revoked(
                    connectionId,
                    usageSessionId,
                    reasonCode
            );

            case "SESSION_ENDED" -> SessionControlMessage.ended(
                    connectionId,
                    usageSessionId,
                    reasonCode
            );

            default -> throw new IllegalArgumentException("Unsupported message type: " + type);
        };

        try {
            send(socketSession, message);
        } catch (IOException e) {
            remove(socketSession);
        }
    }

    private void send(
            WebSocketSession socketSession,
            SessionControlMessage message
    ) throws IOException {
        String json = objectMapper.writeValueAsString(message);

        synchronized (socketSession) {
            if (socketSession.isOpen()) {
                socketSession.sendMessage(new TextMessage(json));
            }
        }
    }

    private void remove(WebSocketSession socketSession) {
        String connectionId = (String) socketSession
                .getAttributes()
                .get("connectionId");

        if (connectionId == null) {
            return;
        }

        sessionsByConnectionId.remove(connectionId);

        Long usageSessionId = usageSessionByConnectionId.remove(connectionId);

        if (usageSessionId != null) {
            connectionIdByUsageSessionId.remove(usageSessionId);
        }
    }

    private Long extractUserId(URI uri) {
        if (uri == null || uri.getQuery() == null) {
            return null;
        }

        String[] parts = uri.getQuery().split("&");

        for (String part : parts) {
            String[] keyValue = part.split("=", 2);

            if (keyValue.length != 2) {
                continue;
            }

            String key = URLDecoder.decode(
                    keyValue[0],
                    StandardCharsets.UTF_8
            );

            String value = URLDecoder.decode(
                    keyValue[1],
                    StandardCharsets.UTF_8
            );

            if ("userId".equals(key)) {
                return Long.valueOf(value);
            }
        }

        return null;
    }
}