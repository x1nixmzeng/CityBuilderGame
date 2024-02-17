#pragma once
#include "component.hpp"

#include <entt/entt.hpp>

#include <raylib.h>

#include <optional>
#include <string>
#include <tuple>

// todo: move to header file
#include "navBlockComponent.hpp"

#include "misc/cells.hpp"

// todo: remove this
struct RouteNode {
    CellPos cell = {};
    bool trap = false;
    std::string templateName;

    std::optional<Vector3> cameraTrigger;
    std::optional<std::string> levelTrigger;
    std::optional<std::string> switchTrigger;

    std::string trigger;
    std::vector<Trigger> triggers;
};

struct MetaBlockComponent : public AssignableComponent {

    RouteNode node;

    inline MetaBlockComponent(const RouteNode&& node_)
        : node(node_) {
    }

    inline MetaBlockComponent(const RouteNode& node_)
        : node(node_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<MetaBlockComponent>(entity, node);
    }
};
