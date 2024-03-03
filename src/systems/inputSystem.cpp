#include "systems/inputSystem.hpp"

#include "resources/resourceManager.hpp"

#include "events/laraEvents.hpp"
#include "events/oskEvents.hpp"

#include "components/autoNavComponent.hpp"

#include <raylib.h>

InputSystem::InputSystem(Game* game)
    : System(game) {

    updateMethod = &InputSystem::updatePlayer;

    eventDispatcher.sink<OnLevelSpawned>()
        .connect<&InputSystem::handleOnLevelSpawned>(*this);

    eventDispatcher.sink<OnLaraMoveEvent>()
        .connect<&InputSystem::handleOnLaraMoveEvent>(*this);

    eventDispatcher.sink<OnStartBot>()
        .connect<&InputSystem::handleOnStartBot>(*this);
}

void InputSystem::handleOnLevelSpawned(const OnLevelSpawned& e) {
    levelName = e.name;
    moveCount = 0;
    autoNav.clear();

    // todo: not all levels will have auto-nav enabled. also - debug only?
    AutoNavComponent const* autoNavComponent = registry.try_get<AutoNavComponent>(e.entity);
    if (autoNavComponent) {
        autoNav = autoNavComponent->events;
    }
}

void InputSystem::handleOnLaraMoveEvent(const OnLaraMoveEvent& e) {

    // todo: validate lara position after spamming buttons
    ++moveCount;

    // todo: track current movements
}

void InputSystem::handleOnStartBot(const OnStartBot& e) {
    assert(moveCount == 0);
    updateMethod = &InputSystem::updateBot;
}

void InputSystem::update(float dt) {

    (this->*updateMethod)(dt);
}

bool InputSystem::isLeftPressed() const {
    bool pressed = false;

    if (IsGamepadAvailable(0)) {
        pressed |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    }

    pressed |= game->getKey(KEY_LEFT) == 1;
    return pressed;
}

bool InputSystem::isRightPressed() const {
    bool pressed = false;

    if (IsGamepadAvailable(0)) {
        pressed |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    }

    pressed |= game->getKey(KEY_RIGHT) == 1;
    return pressed;
}

bool InputSystem::isBackwardPressed() const {
    bool pressed = false;

    if (IsGamepadAvailable(0)) {
        pressed |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
    }

    pressed |= game->getKey(KEY_UP) == 1;
    return pressed;
}

bool InputSystem::isForwardPressed() const {
    bool pressed = false;

    if (IsGamepadAvailable(0)) {
        pressed |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
    }

    pressed |= game->getKey(KEY_DOWN) == 1;
    return pressed;
}

bool InputSystem::isInteractPressed() const {
    bool pressed = false;

    if (IsGamepadAvailable(0)) {
        pressed |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }

    pressed |= game->getKey(KEY_SPACE) == 1;
    return pressed;
}

void InputSystem::updatePlayer(float dt) {

    if (isInteractPressed()) {
        OskMoveRequested oskEvent(OskEvent::Interact);
        game->raiseEvent(oskEvent);
    }
    else if (isLeftPressed()) {
        OskMoveRequested oskEvent(OskEvent::MoveLeft);
        game->raiseEvent(oskEvent);
    }
    else if (isRightPressed()) {
        OskMoveRequested oskEvent(OskEvent::MoveRight);
        game->raiseEvent(oskEvent);
    }
    else if (isBackwardPressed()) {
        // todo: depends on screen orientation (the camera may twist around in later levels)
        OskMoveRequested oskEvent(OskEvent::MoveBackward);
        game->raiseEvent(oskEvent);
    }
    else if (isForwardPressed()) {
        OskMoveRequested oskEvent(OskEvent::MoveForward);
        game->raiseEvent(oskEvent);
    }
}

void InputSystem::updateBot(float dt) {

    if (moveCount < autoNav.size()) {
        OskMoveRequested oskEvent(autoNav[moveCount]);
        game->raiseEvent(oskEvent);
    }
}
