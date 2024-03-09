#pragma once
#include "component.hpp"

#include <vector>

#include "misc/cells.hpp"
#include "misc/enemies.hpp"

#include <entt/entt.hpp>

struct BladeComponent : public AssignableComponent {

    const float bladeSpeed = 300.0f;
    CellPos initialCell;
    Vector3 offset;
    MovementPattern pattern;
    int step = 1;

    inline BladeComponent(const CellPos& pos, Vector3 offset_, MovementPattern pattern_)
        : initialCell(pos), offset(offset_), pattern(pattern_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<BladeComponent>(entity, initialCell, offset, pattern);
    }
};
