#pragma once

#include "component.hpp"
#include <raylib.h>

#include "misc/rlights.h"

struct LightComponent : public AssignableComponent {
    Light light;
    bool oldEnabled;

    inline LightComponent(Light light_)
        : light(light_), oldEnabled(light_.enabled) {
     }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<LightComponent>(entity, light);
    }
};
