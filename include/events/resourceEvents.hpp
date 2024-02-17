#pragma once

#include "event.hpp"
#include <string>

struct ResourceUpdatedEvent : public Event {
    std::string id;

    inline ResourceUpdatedEvent(const std::string& eventId)
        : id(eventId) {
    }
};
