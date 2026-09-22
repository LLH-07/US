package vn.ucon.demo.ucon.pip.provider;

public interface AttributeWriter {
    boolean supports(String writerName);

    void write(AttributeUpdateCommand command);
}
