package vn.ucon.demo.ucon.pip.provider;

public interface AttributeUpdateHandler {

    boolean supports(String handlerName);

    void execute(ResolvedAttributeUpdateCommand command);
}