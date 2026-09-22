package vn.ucon.demo.ucon.obligation;

import org.springframework.stereotype.Component;

import java.util.Map;

@Component
public class ConsentObligationHandler
        implements ObligationFulfillmentHandler {

    @Override
    public boolean supports(String obligationType) {
        return "CONSENT".equalsIgnoreCase(obligationType);
    }

    @Override
    public void validate(Map<String, Object> evidence) {
        Object accepted = evidence.get("accepted");

        if (!(accepted instanceof Boolean value) || !value) {
            throw new IllegalArgumentException(
                    "Consent obligation requires accepted=true"
            );
        }
    }
}