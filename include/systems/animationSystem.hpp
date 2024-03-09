#pragma once
#include "systems/system.hpp"

class AnimationSystem : public System {
  public:
    AnimationSystem(Game* game);

    virtual void update(float dt) override;
};
