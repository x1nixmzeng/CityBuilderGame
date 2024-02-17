#pragma once
#include "misc/typedefs.hpp"

#include "misc/cells.hpp"

#include <map>

struct TileNavigation {
    bool allowLeft = true;
    bool allowRight = true;
    bool allowForward = true;
    bool allowBackward = true;
};

struct TileTemplate {
    Surface surface;
    TileNavigation navigation;
    std::string meshName;
};

using TileTemplatePtr = ResourcePtr<TileTemplate>;
