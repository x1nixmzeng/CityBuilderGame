#pragma once
#include <iostream>
#include <string_view>
#include <raylib.h>

struct CellPos {
    int x, y, z;
};

CellPos& operator+=(CellPos& lhs, CellPos const& rhs);
bool operator==(CellPos const& lhs, CellPos const& rhs);
bool operator<(const CellPos& lhs, const CellPos& rhs);

enum class Surface {
    Ground,
    Front,
    Side,
};

Surface SurfaceFromString(std::string_view const& surfaceString);

enum TileType {
    Static_Geo,
    Dynamic_Geo,
};

struct Trigger {
    std::string value;
    CellPos cellOffset{};
};

Vector3 CellToWorldPosition(CellPos const& cellPosition);
