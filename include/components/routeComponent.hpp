#pragma once
#include "component.hpp"

#include <entt/entt.hpp>

#include <map>
#include <optional>
#include <string>
#include <utility>

#include "metaBlockComponent.hpp"

#include "misc/cells.hpp"

struct Enemy {
    CellPos cell = {};
    Vector3 offset = {};
    std::string templateName;
};

struct RouteComponent : public AssignableComponent {

    using RouteNodes = std::vector<RouteNode>;
    using Enemies = std::vector<Enemy>;

    RouteNodes routeNodes;
    Enemies enemies;

    inline RouteComponent(const RouteNodes&& nodes, const Enemies&& enemies_)
        : routeNodes(nodes), enemies(enemies_) {
    }

    inline RouteComponent(const RouteNodes& nodes, const Enemies& enemies_)
        : routeNodes(nodes), enemies(enemies_) {
    }

    inline void assignToEntity(const entt::entity entity, entt::registry& registry) const override {
        registry.emplace<RouteComponent>(entity, routeNodes, enemies);
    }
};
