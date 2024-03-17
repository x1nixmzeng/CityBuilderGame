#pragma once
#include <string_view>

enum class EnemyType {
    Saw,
    Snake,
};

EnemyType EnemyTypeFromString(std::string_view const& enemyString);

enum class MovementPattern {
    ForwardBack,
    LeftRight,
    Forward,
};

MovementPattern MovementPatternFromString(std::string_view const& patternString);
