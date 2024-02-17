#pragma once
#include "component.hpp"

#include <entt/entt.hpp>

#include <vector>

#include "events/oskEvents.hpp"

struct AutoNavComponent : public AssignableComponent {

    using AutoNavEvents = std::vector<OskEvent>;
    AutoNavEvents events;

    inline AutoNavComponent(const AutoNavEvents&& events_)
        : events(events_) {
    }

    inline AutoNavComponent(const AutoNavEvents& events_)
        : events(events_) {
    }

    inline AutoNavComponent(std::initializer_list<OskEvent> events_ = {})
        : events(events_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<AutoNavComponent>(entity, events);
    }
};
