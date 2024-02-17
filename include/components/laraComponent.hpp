#pragma once
#include "component.hpp"

#include <vector>

#include "misc/cells.hpp"

#include <raylib.h>
#include <entt/entt.hpp>

struct LaraComponent : public AssignableComponent {

    CellPos initialCell;
    Vector3 offset;
    Surface surface;

    inline LaraComponent(const CellPos& pos, const Vector3& offset_, const Surface& surface_)
        : initialCell(pos), offset(offset_), surface(surface_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<LaraComponent>(entity, initialCell, offset, surface);
    }
};
