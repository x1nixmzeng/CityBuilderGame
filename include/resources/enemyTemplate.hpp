#pragma once
#include "misc/enemies.hpp"
#include "misc/typedefs.hpp"
#include <string>

struct EnemyTemplate {
    EnemyType type;
    MovementPattern pattern;
    std::string meshName;
};

using EnemyTemplatePtr = ResourcePtr<EnemyTemplate>;
