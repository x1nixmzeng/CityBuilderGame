#pragma once
#include "events/oskEvents.hpp"
#include "systems/system.hpp"

#include <string>

struct OnLevelSpawned;
struct OnLaraMoveEvent;
struct OnStartBot;

class InputSystem : public System {
  protected:

    void updatePlayer(float dt);
    void updateBot(float dt);

    void handleOnLevelSpawned(const OnLevelSpawned& e);
    void handleOnLaraMoveEvent(const OnLaraMoveEvent& e);
    void handleOnStartBot(const OnStartBot& e);

    void (InputSystem::*updateMethod)(float) = nullptr;
    std::string levelName;
    int moveCount = 0;
    std::vector<OskEvent> autoNav;

  public:
    InputSystem(Game* game);

    virtual void update(float dt) override;
};
