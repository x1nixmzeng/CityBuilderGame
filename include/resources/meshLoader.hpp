#pragma once

#include "resources/meshRes.hpp"
#include <string>

class MeshLoader {
  public:
    static MeshResPtr loadMesh(const std::string& filename);
};