#pragma once

#include "entityEvent.hpp"
#include <string>

#include <utility>

#include <misc/cells.hpp>

struct OnLevelSpawned : public EntityEvent {
    std::string name;

    inline OnLevelSpawned(entt::entity entity, const std::string& levelName)
        : EntityEvent(entity), name(levelName) {
    }
};

struct RequestLevelEvent : public Event {
    std::string name;

    inline RequestLevelEvent(const std::string& levelName)
        : name(levelName) {
    }
};

struct RequestLevelRestart : public Event {
};

struct RequestToggleSwitchEvent : public Event {
    std::string name;

    inline RequestToggleSwitchEvent(const std::string& switchName)
        : name(switchName) {
    }
};

struct OnLaraDiedEvent : public Event {
};

struct OnLaraMoveEvent : public Event {
    CellPos pos;
    Surface surface;

    inline OnLaraMoveEvent(CellPos const& pos_, const Surface& surface_)
        : pos(pos_), surface(surface_) {
    }
};

struct OnStartBot : public Event {
};