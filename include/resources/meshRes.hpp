#pragma once
#include "misc/typedefs.hpp"

#include <raylib.h>

#include <string>

struct MeshRes {

  public:
    Model model = {0};
    ModelAnimation* anims = nullptr;
    int animCount = 0;
    float animDelta = 0.0f;
    std::string debugName;

    MeshRes();

    ~MeshRes() {
        // todo: unload mesh
    }
};

using MeshResPtr = ResourcePtr<MeshRes>;
