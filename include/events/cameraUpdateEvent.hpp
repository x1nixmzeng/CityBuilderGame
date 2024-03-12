#pragma once

#include "entityEvent.hpp"
#include <raylib.h>

struct CameraUpdateEvent : public EntityEvent {
    bool sizeUpdated;
    bool positionUpdated;
    bool rotationUpdated;

    inline CameraUpdateEvent(entt::entity entity, bool sizeUpdated, bool positionUpdated, bool rotationUpdated)
        : EntityEvent(entity), sizeUpdated(sizeUpdated), positionUpdated(positionUpdated), rotationUpdated(rotationUpdated) {
    }
};

struct CameraRequestLookAt : public Event {
    Vector3 target;
};

struct CameraRequestPosition : public Event {
    Vector3 target;
};