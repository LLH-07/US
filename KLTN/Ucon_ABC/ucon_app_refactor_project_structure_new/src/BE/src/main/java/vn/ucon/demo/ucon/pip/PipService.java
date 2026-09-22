package vn.ucon.demo.ucon.pip;

import org.springframework.stereotype.Service;
import vn.ucon.demo.ucon.model.AccessRequest;
import vn.ucon.demo.ucon.model.PipAttributeMapping;
import vn.ucon.demo.ucon.model.PolicyAttributeRequirement;
import vn.ucon.demo.ucon.model.ResolvedAttribute;
import vn.ucon.demo.ucon.model.SessionDependency;
import vn.ucon.demo.ucon.model.TriggerEvent;
import vn.ucon.demo.ucon.pip.provider.AttributeReadContext;
import vn.ucon.demo.ucon.pip.provider.AttributeReader;
import vn.ucon.demo.ucon.pip.provider.DependencyResolver;
import vn.ucon.demo.ucon.model.AttributeUpdateDefinition;
import vn.ucon.demo.ucon.pip.provider.AttributeUpdateRequest;
import vn.ucon.demo.ucon.pip.provider.AttributeUpdateHandler;
import vn.ucon.demo.ucon.pip.provider.ResolvedAttributeUpdate;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

@Service
public class PipService {

    private final PipAttributeMappingRepository mappingRepository;
    private final List<AttributeReader> readers;
    private final List<DependencyResolver> dependencyResolvers;
    private final AttributeUpdateDefinitionRepository updateDefinitionRepository;
    private final List<AttributeUpdateHandler> updateHandlers;

    public PipService(
            PipAttributeMappingRepository mappingRepository,
            AttributeUpdateDefinitionRepository updateDefinitionRepository,
            List<AttributeReader> readers,
            List<AttributeUpdateHandler> updateHandlers,
            List<DependencyResolver> dependencyResolvers
    ) {
        this.mappingRepository = mappingRepository;
        this.updateDefinitionRepository = updateDefinitionRepository;
        this.readers = readers;
        this.updateHandlers = updateHandlers;
        this.dependencyResolvers = dependencyResolvers;
    }

    public List<ResolvedAttribute> resolveAttributes(
            AccessRequest request,
            String policyRef,
            List<SessionDependency> dependencies,
            List<PolicyAttributeRequirement> requirements
    ) {
        List<ResolvedAttribute> result = new ArrayList<>();

        for (PolicyAttributeRequirement req : requirements) {
            if (!req.isEvaluationAttribute()) {
                continue;
            }

            PipAttributeMapping mapping = mappingRepository.findByAttrKey(req.attrKey());
            AttributeReader reader = findReader(mapping.readerName());

            Object value = reader.read(new AttributeReadContext(
                    req.attrKey(),
                    request,
                    policyRef,
                    dependencies
            ));

            result.add(new ResolvedAttribute(
                    req.attrKey(),
                    mapping.xacmlAttributeId(),
                    mapping.xacmlCategory(),
                    mapping.xacmlDataType(),
                    value
            ));
        }

        return result;
    }

    public List<SessionDependency> resolveDependencies(
            AccessRequest request,
            List<PolicyAttributeRequirement> requirements
    ) {
        Map<String, SessionDependency> unique = new LinkedHashMap<>();

        for (PolicyAttributeRequirement req : requirements) {
            if (!req.isEvaluationAttribute()) {
                continue;
            }

            PipAttributeMapping mapping = mappingRepository.findByAttrKey(req.attrKey());

            if ("NONE".equalsIgnoreCase(mapping.monitorType())) {
                continue;
            }

            if (mapping.dependencyName() == null || mapping.dependencyName().isBlank()) {
                continue;
            }

            DependencyResolver resolver = findDependencyResolver(mapping.dependencyName());

            for (SessionDependency raw : resolver.resolve(req.attrKey(), request)) {
                SessionDependency dependency = new SessionDependency(
                        raw.dependencyKey(),
                        req.attrKey(),
                        raw.entityType(),
                        raw.entityId(),
                        raw.attributeName(),
                        mapping.monitorType(),
                        raw.nextCheckAt()
                );

                String key = dependency.dependencyKey()
                        + "|"
                        + dependency.attrKey()
                        + "|"
                        + dependency.entityType()
                        + "|"
                        + dependency.entityId()
                        + "|"
                        + dependency.attributeName();

                unique.put(key, dependency);
            }
        }

        return new ArrayList<>(unique.values());
    }



    public boolean applyAttributeUpdate(
            AttributeUpdateRequest request,
            TriggerEvent triggerEvent
    ) {
        AttributeUpdateDefinition definition =
                updateDefinitionRepository.findEnabledByActionCode(request.actionCode());
        PipAttributeMapping mapping = mappingRepository.findByAttrKey(definition.attrKey());

        if (shouldSkipBecauseTargetEntityDeleted(definition.attrKey(), triggerEvent)) {
            return false;
        }

        if (!mapping.mutable()) {
            throw new IllegalStateException(
                    "Attribute is not mutable: " + definition.attrKey()
            );
        }

        AttributeUpdateHandler handler = findUpdateHandler(definition.handlerName());

        Object value = request.value() != null
                ? request.value()
                : parseDefaultValue(definition.defaultValue());

        handler.apply(new ResolvedAttributeUpdate(
                definition.actionCode(),
                definition.attrKey(),
                definition.operation(),
                value,
                request.usageSessionId()
        ));

        return true;
    }

    private AttributeReader findReader(String readerName) {
        return readers.stream()
                .filter(reader -> reader.supports(readerName))
                .findFirst()
                .orElseThrow(() -> new IllegalArgumentException(
                        "Unsupported attribute reader: " + readerName
                ));
    }

    private DependencyResolver findDependencyResolver(String dependencyName) {
        return dependencyResolvers.stream()
                .filter(resolver -> resolver.supports(dependencyName))
                .findFirst()
                .orElseThrow(() -> new IllegalArgumentException(
                        "Unsupported dependency resolver: " + dependencyName
                ));
    }

    private AttributeUpdateHandler findUpdateHandler(String handlerName) {
        return updateHandlers.stream()
                .filter(handler -> handler.supports(handlerName))
                .findFirst()
                .orElseThrow(() -> new IllegalArgumentException(
                        "Unsupported update handler: " + handlerName
                ));
    }

    private boolean shouldSkipBecauseTargetEntityDeleted(
            String attrKey,
            TriggerEvent triggerEvent
    ) {
        if (triggerEvent == null) {
            return false;
        }

        if (!"ENTITY_DELETED".equalsIgnoreCase(triggerEvent.eventType())) {
            return false;
        }

        String targetEntityType = entityTypeFromAttrKey(attrKey);

        return targetEntityType != null
                && targetEntityType.equalsIgnoreCase(triggerEvent.entityType());
    }

    private String entityTypeFromAttrKey(String attrKey) {
        if (attrKey == null || attrKey.isBlank()) {
            return null;
        }

        int dotIndex = attrKey.indexOf('.');

        if (dotIndex <= 0) {
            return null;
        }

        return attrKey.substring(0, dotIndex).toUpperCase();
    }

    private Object parseDefaultValue(String value) {
        if (value == null || value.isBlank()) {
            return null;
        }

        try {
            return Long.parseLong(value);
        } catch (NumberFormatException ex) {
            return value;
        }
    }
}
