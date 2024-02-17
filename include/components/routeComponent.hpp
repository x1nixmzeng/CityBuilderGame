#pragma once
#include "component.hpp"

#include <entt/entt.hpp>

#include <map>
#include <optional>
#include <string>
#include <utility>

#include "metaBlockComponent.hpp"

#include "misc/cells.hpp"

struct RouteComponent : public AssignableComponent {

    using RouteNodes = std::vector<RouteNode>;

    RouteNodes routeNodes;

    inline RouteComponent(const RouteNodes&& nodes)
        : routeNodes(nodes) {
    }

    inline RouteComponent(const RouteNodes& nodes)
        : routeNodes(nodes) {
    }

    inline RouteComponent(std::initializer_list<RouteNode> nodes = {})
        : routeNodes(nodes) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<RouteComponent>(entity, routeNodes);
    }
};
