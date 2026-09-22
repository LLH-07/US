package vn.ucon.demo.ucon.pip.provider;

import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.SessionDependency;

import java.util.List;

public interface DependencyResolver {

    boolean supports(String dependencyName);

    List<SessionDependency> resolve(
            String attrKey,
            AccessRequest request
    );
}
