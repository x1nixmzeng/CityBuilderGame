#pragma once
#include <iostream>
#include <raylib.h>
#include <tuple>

struct CellPos {
    int x, y, z;
};

CellPos& operator+=(CellPos& lhs, CellPos const& rhs);
bool operator==(CellPos const& lhs, CellPos const& rhs);

enum class Surface {
    Ground,
    Wall_Front,
    Wall_Side,
};

Surface SurfaceFromString(std::string const& surfaceString);

enum TileType {
    Static_Geo,
    Dynamic_Geo,
};

struct Trigger {
    std::string value;
    CellPos cellOffset{};
};

Vector3 CellToWorldPosition(CellPos const& cellPosition);
