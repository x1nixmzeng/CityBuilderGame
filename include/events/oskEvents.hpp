#pragma once

#include "event.hpp"

enum class OskEvent {
    MoveLeft,
    MoveRight,
    MoveForward,
    MoveBackward,
    Interact,
};

struct OskMoveRequested : public Event {
    OskEvent oskEvent;

    inline OskMoveRequested(const OskEvent& event)
        : oskEvent(event) {
    }
};
