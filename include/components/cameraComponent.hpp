#pragma once

#include "component.hpp"
#include <raylib.h>

struct CameraComponent : public Component<false> {

    Camera camera = {0};
};

struct CameraTargetComponent : public Component<false> {
    Vector3 target;
    Vector3 up;
    bool valid = false;
};
