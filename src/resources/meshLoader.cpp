#include "resources/meshLoader.hpp"

#include "resources/rlobj.h"

#include <cassert>

MeshResPtr MeshLoader::loadMesh(const std::string& filename) {
    MeshRes* mesh = new MeshRes();

    // instead use https://github.com/Not-Nik/rlobj
    mesh->model = LoadObj(filename.c_str());

    // models will silently fail to load if files cannot be found
    assert(mesh->model.meshCount > 0);

    return MeshResPtr(mesh);
}
