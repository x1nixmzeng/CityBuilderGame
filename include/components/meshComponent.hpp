#pragma once
#include "component.hpp"
#include "resources/meshRes.hpp"

#include <raylib.h>

struct MeshComponent : public AssignableComponent {
    MeshResPtr mesh;

    inline MeshComponent(const MeshResPtr& mesh)
        : mesh(mesh) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<MeshComponent>(entity, mesh);
    }    
};
