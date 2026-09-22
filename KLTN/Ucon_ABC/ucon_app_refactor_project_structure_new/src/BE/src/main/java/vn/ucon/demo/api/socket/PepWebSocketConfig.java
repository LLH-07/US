package vn.ucon.demo.api.socket;

import org.springframework.context.annotation.Configuration;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.WebSocketConfigurer;
import org.springframework.web.socket.config.annotation.WebSocketHandlerRegistry;

@Configuration
@EnableWebSocket
public class PepWebSocketConfig implements WebSocketConfigurer {

    private final PepControlSocketHandler controlSocketHandler;

    public PepWebSocketConfig(PepControlSocketHandler controlSocketHandler) {
        this.controlSocketHandler = controlSocketHandler;
    }

    @Override
    public void registerWebSocketHandlers(WebSocketHandlerRegistry registry) {
        registry.addHandler(controlSocketHandler, "/ws/pep/control")
                .setAllowedOriginPatterns("*");
    }
}