#include "misc/enemies.hpp"

EnemyType EnemyTypeFromString(std::string_view const& enemyString) {
    if (enemyString == "saw") {
        return EnemyType::Saw;
    }
    else if (enemyString == "snake") {
        return EnemyType::Snake;
    }

    std::unreachable();
}

MovementPattern MovementPatternFromString(std::string_view const& patternString) {
    if (patternString == "forwardback") {
        return MovementPattern::ForwardBack;
    }
    else if (patternString == "leftright") {
        return MovementPattern::LeftRight;
    }
    else if (patternString == "forward") {
        return MovementPattern::Forward;
    }

    std::unreachable();
}
