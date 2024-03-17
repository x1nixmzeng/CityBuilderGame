#pragma once
#include "component.hpp"

#include <vector>

#include "misc/cells.hpp"
#include "misc/enemies.hpp"

#include <entt/entt.hpp>

struct SnakeComponent : public AssignableComponent {

    CellPos initialCell;
    MovementPattern pattern;
    Surface surface;

    inline SnakeComponent(const CellPos& pos, MovementPattern const& pattern_, Surface const& surface_)
        : initialCell(pos), pattern(pattern_), surface(surface_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<SnakeComponent>(entity, initialCell, pattern, surface);
    }
};
