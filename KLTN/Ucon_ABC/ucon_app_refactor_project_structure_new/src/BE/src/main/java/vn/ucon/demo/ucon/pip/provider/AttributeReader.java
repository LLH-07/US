package vn.ucon.demo.ucon.pip.provider;

public interface AttributeReader {
    boolean supports(String readerName);

    Object read(AttributeReadContext context);
}