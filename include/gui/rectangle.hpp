#pragma once

#include <raylib.h>

inline bool pointInside(Rectangle const& rect, float x, float y) {
    return x >= rect.x && x <= rect.x + rect.width &&
           y >= rect.y && y <= rect.y + rect.height;
}
