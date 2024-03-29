#include "resources/meshLoader.hpp"

#include "resources/rlobj.h"

#include <cassert>
#include <filesystem>

MeshResPtr MeshLoader::loadMesh(const std::string& filename) {
    MeshRes* mesh = new MeshRes();

    assert(std::filesystem::exists(filename));

    auto ext = std::filesystem::path(filename).extension();
    if (ext == ".obj") {
        // instead use https://github.com/Not-Nik/rlobj
        mesh->model = LoadObj(filename.c_str());
    }
    else {
        mesh->model = LoadModel(filename.c_str());
        mesh->anims = LoadModelAnimations(filename.c_str(), &mesh->animCount);
    }

    // models must have at least one mesh
    assert(mesh->model.meshCount > 0);

    return MeshResPtr(mesh);
}
