package vn.ucon.demo.ucon.pip.provider;

public interface AttributeUpdateHandler {

    boolean supports(String handlerName);

    void apply(ResolvedAttributeUpdate update);
}
