#include "game.hpp"

#include "application.hpp"
#include "components/components.hpp"
#include "events/events.hpp"
#include "systems/systems.hpp"
#include "gui/gui.hpp"
#include "gui/menus.hpp"

Game::Game(Application* app)
    : app(app), resourceManager("res/") {
    logStream = std::ofstream("log.txt");

    init();
}

void Game::init() {
    // init systems
    systems.push_back(new CameraSystem(this));
    camera = registry.view<CameraComponent>().front();

    renderSystem = new RenderSystem(this);
    systems.push_back(renderSystem);

    systems.push_back(new LevelSystem(this));
    systems.push_back(new MovementSystem(this));
    systems.push_back(new AnimationSystem(this));
    systems.push_back(new InputSystem(this));
}

entt::registry& Game::getRegistry() {
    return registry;
}

entt::dispatcher& Game::getEventDispatcher() {
    return eventDispatcher;
}

ResourceManager& Game::getResourceManager() {
    return resourceManager;
}

void Game::update(float dt) {
    if (state == GameState::PAUSED) {
        // explicitly update just the render system when paused
        renderSystem->update(dt);
    }
    else {
        for (System* system : systems) {
            system->update(dt);
        }
    }
}

void Game::reloadResources() {
    resourceManager.loadResources();
}

int Game::getKey(int key) const {
    return IsKeyPressed(key);
}

Vector2 Game::getMousePos() const {
    return GetMousePosition();
}

void Game::setState(GameState state) {
    this->state = state;
}

GameState Game::getState() const {
    return state;
}

// handle framebuffer size and camera events even if the game is paused
template<>
void Game::raiseEvent<FramebufferSizeEvent>(FramebufferSizeEvent& e) {
    eventDispatcher.trigger<FramebufferSizeEvent&>(e);
}

template<>
void Game::raiseEvent<CameraUpdateEvent>(CameraUpdateEvent& e) {
    eventDispatcher.trigger<CameraUpdateEvent&>(e);
}

template<>
void Game::raiseEvent<OnLevelSpawned>(OnLevelSpawned& e) {
    eventDispatcher.trigger<OnLevelSpawned&>(e);
}

template<>
void Game::raiseEvent<RequestLevelEvent>(RequestLevelEvent& e) {
    eventDispatcher.trigger<RequestLevelEvent&>(e);
}

template<>
void Game::raiseEvent<RequestLevelRestart>(RequestLevelRestart& e) {
    eventDispatcher.trigger<RequestLevelRestart&>(e);
}

template<>
void Game::raiseEvent<OnLaraDiedEvent>(OnLaraDiedEvent& e) {
    eventDispatcher.trigger<OnLaraDiedEvent&>(e);
}

template<>
void Game::raiseEvent<OnStartBot>(OnStartBot& e) {
    eventDispatcher.trigger<OnStartBot&>(e);
}

template<typename Event>
void Game::raiseEvent(Event& e) {
    if (state != GameState::PAUSED) {
        eventDispatcher.trigger<Event&>(e);
    }
}

template void Game::raiseEvent<KeyEvent>(KeyEvent&);
template void Game::raiseEvent<MouseButtonEvent>(MouseButtonEvent&);
template void Game::raiseEvent<MouseMoveEvent>(MouseMoveEvent&);
template void Game::raiseEvent<MouseScrollEvent>(MouseScrollEvent&);
template void Game::raiseEvent<ResourceUpdatedEvent>(ResourceUpdatedEvent&);
template void Game::raiseEvent<OskMoveRequested>(OskMoveRequested&);
template void Game::raiseEvent<CameraRequestLookAt>(CameraRequestLookAt&);
template void Game::raiseEvent<RequestToggleSwitchEvent>(RequestToggleSwitchEvent&);
template void Game::raiseEvent<OnLaraMoveEvent>(OnLaraMoveEvent&);

#if DEBUG
void Game::log(const std::string& message) {
    logStream << message << std::endl;
}
#endif
