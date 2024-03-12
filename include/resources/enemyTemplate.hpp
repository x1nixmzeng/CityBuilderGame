#pragma once
#include "misc/cells.hpp"
#include "misc/enemies.hpp"
#include "misc/typedefs.hpp"
#include <string>

struct EnemyTemplate {
    EnemyType type;
    MovementPattern pattern;
    Surface surface;
    std::string meshName;
};

using EnemyTemplatePtr = ResourcePtr<EnemyTemplate>;
