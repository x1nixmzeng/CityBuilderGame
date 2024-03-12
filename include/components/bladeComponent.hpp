#pragma once
#include "component.hpp"

#include <vector>

#include "misc/cells.hpp"
#include "misc/enemies.hpp"

#include <entt/entt.hpp>

struct BladeComponent : public AssignableComponent {

    const float bladeSpeed = 300.0f;
    CellPos initialCell;
    MovementPattern pattern;
    Surface surface;
    int step = 1;

    inline BladeComponent(const CellPos& pos, MovementPattern const& pattern_, Surface const& surface_)
        : initialCell(pos), pattern(pattern_), surface(surface_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<BladeComponent>(entity, initialCell, pattern, surface);
    }
};
