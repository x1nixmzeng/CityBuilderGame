#pragma once
#include <string_view>

enum class EnemyType {
    Saw,
};

EnemyType EnemyTypeFromString(std::string_view const& enemyString);

enum class MovementPattern {
    ForwardBack,
    LeftRight,
};

MovementPattern MovementPatternFromString(std::string_view const& patternString);
