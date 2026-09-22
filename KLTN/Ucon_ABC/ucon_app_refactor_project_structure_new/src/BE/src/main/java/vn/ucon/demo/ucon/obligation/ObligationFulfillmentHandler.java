package vn.ucon.demo.ucon.obligation;

import java.util.Map;

public interface ObligationFulfillmentHandler {

    boolean supports(String obligationType);

    void validate(Map<String, Object> evidence);
}