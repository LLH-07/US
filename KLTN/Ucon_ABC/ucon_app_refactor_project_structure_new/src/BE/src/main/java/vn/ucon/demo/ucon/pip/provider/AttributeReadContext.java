package vn.ucon.demo.ucon.pip.provider;

import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;

import java.util.List;

public record AttributeReadContext(
        String attrKey,
        AccessRequest request,
        String policyRef,
        List<SessionDependency> dependencies
) {}