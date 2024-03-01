#include "resources/meshLoader.hpp"

#include "resources/rlobj.h"

#include <cassert>
#include <filesystem>

MeshResPtr MeshLoader::loadMesh(const std::string& filename) {
    MeshRes* mesh = new MeshRes();

    // instead use https://github.com/Not-Nik/rlobj
    mesh->model = LoadObj(filename.c_str());
    //mesh->debugName = std::filesystem::path(filename).filename().string();

    // models will silently fail to load if files cannot be found
    assert(mesh->model.meshCount > 0);

    return MeshResPtr(mesh);
}
