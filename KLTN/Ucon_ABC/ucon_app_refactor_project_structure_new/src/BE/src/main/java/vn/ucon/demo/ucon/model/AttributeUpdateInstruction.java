package vn.ucon.demo.ucon.model;

/**
 * Internal UCON instruction representing one system-controlled attribute update.
 *
 * The PDP adapter may decode this instruction from a XACML PEP action, but the
 * rest of the UCON core does not treat it as a subject obligation.
 */
public record AttributeUpdateInstruction(
        String actionCode
) {}
