#pragma once
#include "system.hpp"

#include "misc/buildingType.hpp"

#include <glm/glm.hpp>

struct MouseButtonEvent;
struct KeyEvent;
struct BuildEvent;

class BuildSystem : public System {
  protected:
    virtual void init() override;

    entt::entity cameraEntity;
    entt::entity buildMarkerEntity;

    struct BuildSystemState {
        // building process ongoing
        bool building = false;

        // start position of the current building process
        glm::ivec2 startPosition;

        // selected building type
        BuildingType selectedBuildingType = BuildingType::CLEAR;
    };

    BuildSystemState state{};

    glm::ivec2 getGridPos(const glm::vec2& mousePos) const;

    void setState(BuildingType currentBuildingType, bool building = false, const glm::ivec2& startPosition = glm::ivec2(-1));

  public:
    BuildSystem(Game* game);

    virtual void update(float dt) override;

    void handleMouseButtonEvent(const MouseButtonEvent& e);

    void handleKeyEvent(const KeyEvent& e);

    void handleBuildEvent(const BuildEvent& e);
};
