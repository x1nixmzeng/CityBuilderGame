#pragma once
#include "component.hpp"

#include "misc/cells.hpp"

enum class NavBlockType {
    Standard,
    Trap,
};

struct NavBlocks {
    bool allowLeft = true;
    bool allowRight = true;
    bool allowForward = true;
    bool allowBackward = true;
};

struct NavBlockData {
    CellPos cell;
    Surface surface;
    NavBlockType type;
    NavBlocks blocks;
    int steps = 0;
    bool allowSaw;
};

struct NavBlockComponent : public AssignableComponent {
    NavBlockData data;

    inline NavBlockComponent()
        : data() {
    }

    inline NavBlockComponent(NavBlockData const& data_)
        : data(data_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<NavBlockComponent>(entity, data);
    }
};
