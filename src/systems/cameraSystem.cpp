#include "systems/cameraSystem.hpp"

#include "components/components.hpp"
#include "events/cameraUpdateEvent.hpp"

#include <iostream>
#include <raylib.h>

void CameraSystem::init() {
    cameraEntity = registry.create();

    CameraComponent& cameraComponent = registry.emplace<CameraComponent>(cameraEntity);
    // Initial position is here
    cameraComponent.camera.position = {5.0f, 10.0f, 20.0f}; // Camera position
    cameraComponent.camera.target = {0.0f, 0.4f, 0.0f};     // Camera looking at point
    cameraComponent.camera.up = {0.0f, 1.0f, 0.0f};         // Camera up vector (rotation towards target)
    cameraComponent.camera.fovy = 45.0f;                    // Camera field-of-view Y
    cameraComponent.camera.projection = CAMERA_PERSPECTIVE; // Camera projection type
}

CameraSystem::CameraSystem(Game* game)
    : System(game) {

    init();

    eventDispatcher.sink<CameraRequestLookAt>()
        .connect<&CameraSystem::handleCameraRequestLookAt>(*this);

    eventDispatcher.sink<CameraRequestPosition>()
        .connect<&CameraSystem::handleCameraRequestPosition>(*this);
}

void CameraSystem::update(float dt) {

    CameraComponent& cameraComponent = registry.get<CameraComponent>(cameraEntity);

    // only when the control key is down
    if (IsKeyDown(KEY_C)) {
        UpdateCamera(&cameraComponent.camera, CAMERA_FREE);
    }
}

void CameraSystem::handleCameraRequestLookAt(const CameraRequestLookAt& e) {

    CameraComponent& cameraComponent = registry.get<CameraComponent>(cameraEntity);
    cameraComponent.camera.target = e.target;
}

void CameraSystem::handleCameraRequestPosition(const CameraRequestPosition& e) {
    CameraComponent& cameraComponent = registry.get<CameraComponent>(cameraEntity);
    cameraComponent.camera.position = e.target;
}
