#pragma once
#include "meshComponent.hpp"

#include "resources/meshRes.hpp"
#include "components/transformationComponent.hpp"

#include <vector>

struct InstancedMeshComponent : public MeshComponent { 

    std::vector<TransformationComponent> transformations;

    inline InstancedMeshComponent(const MeshResPtr& mesh, const std::vector<TransformationComponent>& transformations_)
        : MeshComponent(mesh), transformations(transformations_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<InstancedMeshComponent>(entity, mesh, transformations);
    }
};
