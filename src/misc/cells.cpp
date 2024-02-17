#include "misc/cells.hpp"

#include <raymath.h>

CellPos& operator+=(CellPos& lhs, CellPos const& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

bool operator==(CellPos const& lhs, CellPos const& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

Surface SurfaceFromString(std::string const& surfaceString) {
    if (surfaceString == "ground") {
        return Surface::Ground;
    }
    else if (surfaceString == "wall_front") {
        return Surface::Wall_Front;
    }
    else if (surfaceString == "wall_side") {
        return Surface::Wall_Side;
    }
    else if (surfaceString.size() > 0) {
        // assert(false && "Unsupported surface or typo in surface attribute");
    }

    return Surface::Ground;
}

Vector3 CellToWorldPosition(CellPos const& cellPosition) {

    Vector3 asVector3;
    asVector3.x = cellPosition.x;
    asVector3.y = cellPosition.y;
    asVector3.z = cellPosition.z;
    return Vector3Scale(asVector3, 2.0f);
}
