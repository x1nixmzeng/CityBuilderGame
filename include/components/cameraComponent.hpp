#pragma once

#include "component.hpp"
#include <raylib.h>

struct CameraComponent : public Component<false> {
    Camera camera = {0};
};
