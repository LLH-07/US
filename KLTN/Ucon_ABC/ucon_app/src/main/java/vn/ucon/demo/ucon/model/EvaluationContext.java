package vn.ucon.demo.ucon.model;

import java.util.List;

public record EvaluationContext(
        AccessRequest accessRequest,
        PolicyMetadata policyMetadata,
        List<SessionDependency> dependencies,
        List<ResolvedAttribute> attributes
) {}