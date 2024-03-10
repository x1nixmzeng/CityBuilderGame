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
    int step = 1;

    inline BladeComponent(const CellPos& pos, MovementPattern pattern_)
        : initialCell(pos), pattern(pattern_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<BladeComponent>(entity, initialCell, pattern);
    }
};
