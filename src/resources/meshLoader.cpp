#include "resources/meshLoader.hpp"

#include "resources/rlobj.h"

MeshResPtr MeshLoader::loadMesh(const std::string& filename) {
    MeshRes* mesh = new MeshRes();

    // instead use https://github.com/Not-Nik/rlobj
    mesh->model = LoadObj(filename.c_str());

    return MeshResPtr(mesh);
}
