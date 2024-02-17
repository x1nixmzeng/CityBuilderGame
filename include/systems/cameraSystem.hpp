#pragma once
#include "systems/system.hpp"

#include "events/framebufferSizeEvent.hpp"
#include "events/mouseEvents.hpp"

#include <raylib.h>

struct CameraRequestLookAt;

class CameraSystem : public System {
  protected:
    entt::entity cameraEntity;

    virtual void init() override;

  public:
    CameraSystem(Game* game);

    virtual void update(float dt) override;

    void handleCameraRequestLookAt(const CameraRequestLookAt& e);
};