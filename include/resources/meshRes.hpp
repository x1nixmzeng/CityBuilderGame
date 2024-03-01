#pragma once
#include "misc/typedefs.hpp"

#include <raylib.h>

#include <string>

struct MeshRes {

  public:
    Model model = {0};
    std::string debugName;

    MeshRes();

    ~MeshRes() {
        // todo: unload mesh
    }
};

using MeshResPtr = ResourcePtr<MeshRes>;
