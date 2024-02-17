#pragma once
#include "component.hpp"

#include <utility>

struct BuildMarkerComponent : public Component<false> {
    /// @brief Current position of the build marker in normalized world grid coordinates
    std::pair<int, int> position;
    /// @brief If true the build marker is rendered otherwise it is not rendered
    bool visible;

    inline BuildMarkerComponent(const std::pair<int, int>& position, bool visible = false)
        : position(position), visible(visible) {
    }
};
