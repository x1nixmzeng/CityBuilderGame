#pragma once
#include "resources/resourceManager.hpp"

#include "misc/typedefs.hpp"

#include <entt/entt.hpp>

#include <fstream>
#include <raylib.h>

class System;
class Application;
class RenderSystem;

enum class GameState {
    PAUSED,
    RUNNING,
};

class Game {
    std::vector<System*> systems;

    ResourceManager resourceManager;

    entt::registry registry;
    entt::dispatcher eventDispatcher;

    Application* app;

    void init();

    GameState state = GameState::RUNNING;

#if DEBUG
    std::ofstream logStream;
#endif

  public:
    entt::entity camera;
    RenderSystem* renderSystem;

    Game(Application* app);

    entt::registry& getRegistry();
    entt::dispatcher& getEventDispatcher();
    ResourceManager& getResourceManager();

    void update(float dt);
    void reloadResources();

    int getKey(int key) const;

    Vector2 getMousePos() const;

    void setState(GameState state);
    GameState getState() const;

    template<typename Event>
    void raiseEvent(Event& args);

#if DEBUG
    void log(const std::string& message);
#endif
};
