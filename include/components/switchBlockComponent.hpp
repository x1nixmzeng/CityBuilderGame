#pragma once
#include "component.hpp"

#include "misc/cells.hpp"

#include <entt/entt.hpp>
#include <raylib.h>

#include <string>

struct SwitchBlockComponent : public AssignableComponent {

    std::string key;
    std::string state;
    Vector3 position;

    inline SwitchBlockComponent(std::string const& key_, std::string const& state_, Vector3 const& position_)
        : key(key_), state(state_), position(position_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<SwitchBlockComponent>(entity, key, state, position);
    }
};
