#pragma once
#include "event.hpp"

enum class KeyAction {
    Press
};

struct KeyEvent : public Event {
    int key;
    int scancode;
    KeyAction action;
    int mods;

    inline KeyEvent(int key, int scancode, KeyAction action, int mods)
        : key(key), scancode(scancode), action(action), mods(mods) {
    }
};