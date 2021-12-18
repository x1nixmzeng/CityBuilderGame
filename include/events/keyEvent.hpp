#pragma once
#include "event.hpp"

struct KeyEvent : public Event {
    int key;
    int scancode;
    int action;
    int mods;
};