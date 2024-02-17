#pragma once
#include "system.hpp"

#include <raylib.h>
#include <vector>

struct ResourceUpdatedEvent;
struct RequestLevelEvent;
struct RequestLevelRestart;
struct RequestToggleSwitchEvent;

class LevelSystem : public System {
  protected:
    void generateLevel();

    void handleResourceUpdatedEvent(const ResourceUpdatedEvent& e);
    void handleRequestLevelEvent(const RequestLevelEvent& e);
    void handleRequestLevelRestart(const RequestLevelRestart& e);
    void handleRequestToggleSwitchEvent(const RequestToggleSwitchEvent& e);

    bool pendingCreate = false;

    std::vector<entt::entity> dynamicEntities;

    std::string levelName;

    void addSwitchOn(const entt::entity& entity, const Vector3& pos);
    void addSwitchOff(const entt::entity& entity, const Vector3& pos);

  public:
    LevelSystem(Game* game);

    void init() override;
    void update(float dt) override;
};
