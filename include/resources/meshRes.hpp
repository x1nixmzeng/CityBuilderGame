#pragma once
#include "misc/typedefs.hpp"

#include <raylib.h>

struct MeshRes {

  public:
    Model model = {0};

    MeshRes();

    ~MeshRes() {
        // todo: unload mesh
    }
};

using MeshResPtr = ResourcePtr<MeshRes>;
