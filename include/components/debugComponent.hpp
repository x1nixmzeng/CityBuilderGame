#pragma once
#include "component.hpp"

struct DebugComponent : public Component<false> {
    
};

struct NoHitTestComponent : public Component<false> {
};
