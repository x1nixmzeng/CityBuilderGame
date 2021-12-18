#pragma once

#include "application.hpp"

#include <entt/entt.hpp>

    class System {
      protected:
        entt::registry& registry;
        entt::dispatcher& eventDispatcher;

        Application* app;

        virtual void init();

      public:
        System(Application* app);

        virtual void update(int dt);
    };