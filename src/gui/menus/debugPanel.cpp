#include "gui/menus/debugPanel.hpp"

#include "gui/components/button.hpp"
#include "gui/gui.hpp"

#include "application.hpp"

#include "events/resourceEvents.hpp"

#include "components/cameraComponent.hpp"

#include <format>
#include <raymath.h>

DebugPanel::DebugPanel(Gui* gui)
    : StackPanel("debug_menu", gui, StackOrientation::COLUMN, colors::anthraziteGrey, ItemAligment::BEGIN) {
    constraints.x = AbsoluteConstraint(0);
    constraints.y = AbsoluteConstraint(0);
    constraints.width = RelativeConstraint(0.3);
    cornerRadius = 0.0f;

    TextButton* reloadResourcesButton = new TextButton("debug_menu.reloadResourcesButton", gui, colors::anthraziteGrey, "Reload Resources");
    reloadResourcesButton->constraints.height = AbsoluteConstraint(30);
    reloadResourcesButton->constraints.width = RelativeConstraint(0.9);
    reloadResourcesButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        app->getGame()->reloadResources();

        ResourceUpdatedEvent event{""};
        app->getGame()->raiseEvent<ResourceUpdatedEvent>(event);
    };
    addChild(reloadResourcesButton);

    Label* fpsCounter = new Label("debug_menu.fpsCounter", gui, colors::transparent, "FPS: ");
    fpsCounter->textAlign = TextAlign::BEGIN;
    fpsCounter->constraints.height = AbsoluteConstraint(30);
    fpsCounter->constraints.width = RelativeConstraint(0.9);
    addChild(fpsCounter);

    Label* sunDirection = new Label("debug_menu.sunDirection", gui, colors::transparent, "");
    sunDirection->textAlign = TextAlign::BEGIN;
    sunDirection->constraints.height = AbsoluteConstraint(30);
    sunDirection->constraints.width = RelativeConstraint(0.9);
    addChild(sunDirection);

    Label* sunPosition = new Label("debug_menu.sunPosition", gui, colors::transparent, "");
    sunPosition->textAlign = TextAlign::BEGIN;
    sunPosition->constraints.height = AbsoluteConstraint(30);
    sunPosition->constraints.width = RelativeConstraint(0.9);
    addChild(sunPosition);

    Label* cameraPos = new Label("debug_menu.cameraPosition", gui, colors::transparent, "");
    cameraPos->textAlign = TextAlign::BEGIN;
    cameraPos->constraints.height = AbsoluteConstraint(30);
    cameraPos->constraints.width = RelativeConstraint(0.9);
    addChild(cameraPos);

    Label* cameraPitch = new Label("debug_menu.cameraPitch", gui, colors::transparent, "");
    cameraPitch->textAlign = TextAlign::BEGIN;
    cameraPitch->constraints.height = AbsoluteConstraint(30);
    cameraPitch->constraints.width = RelativeConstraint(0.9);
    addChild(cameraPitch);

    Label* cameraYaw = new Label("debug_menu.cameraYaw", gui, colors::transparent, "");
    cameraYaw->textAlign = TextAlign::BEGIN;
    cameraYaw->constraints.height = AbsoluteConstraint(30);
    cameraYaw->constraints.width = RelativeConstraint(0.9);
    addChild(cameraYaw);
}

void DebugPanel::update() {
    const Application* app = gui->getApp();
    Game* game = app->getGame();
    const entt::registry& registry = game->getRegistry();

    // fps
    float fps = 1.0f / app->updateTime;

    Label* fpsCounter = dynamic_cast<Label*>(getChild("debug_menu.fpsCounter"));
    fpsCounter->text = "FPS: " + std::to_string(fps);

    // camera info
    const CameraComponent& cameraTransform = registry.get<CameraComponent>(game->camera);

    auto const& cameraPos = cameraTransform.camera.position;

    Label* cameraPosition = dynamic_cast<Label*>(getChild("debug_menu.cameraPosition"));
    cameraPosition->text = std::format("Camera position: ({0}, {1}, {2})", cameraPos.x, cameraPos.y, cameraPos.z);

    // const CameraComponent& cameraComponent = registry.get<CameraComponent>(game->camera);

    /*Label* cameraPitch = dynamic_cast<Label*>(getChild("debug_menu.cameraPitch"));
    cameraPitch->text = "Camera pitch: (" + std::to_string(cameraComponent.camera.) + ")";

    Label* cameraYaw = dynamic_cast<Label*>(getChild("debug_menu.cameraYaw"));
    cameraYaw->text = "Camera yaw: (" + std::to_string(cameraComponent.yaw) + ")";*/
}