#include "gui/menus/controlsPanel.hpp"

#include "gui/components/button.hpp"
#include "gui/gui.hpp"

#include "application.hpp"
#include "game.hpp"

#include "events/oskEvents.hpp"

#include "application.hpp"

ControlsPanel::ControlsPanel(Gui* gui)
    : StackPanel("controls_menu", gui, StackOrientation::COLUMN, colors::anthraziteGrey, ItemAligment::BEGIN) {
    constraints.x = AbsoluteConstraint(0);
    constraints.y = AbsoluteConstraint(0);
    constraints.width = RelativeConstraint(0.3);
    cornerRadius = 0.0f;

    TextButton* leftButton = new TextButton("controls_menu.move_left", gui, colors::anthraziteGrey, "Left");
    leftButton->constraints.height = AbsoluteConstraint(30);
    leftButton->constraints.width = RelativeConstraint(0.9);
    leftButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        OskMoveRequested oskEvent(OskEvent::MoveLeft);
        app->getGame()->raiseEvent(oskEvent);
    };
    addChild(leftButton);

    TextButton* rightButton = new TextButton("controls_menu.move_right", gui, colors::anthraziteGrey, "Right");
    rightButton->constraints.height = AbsoluteConstraint(30);
    rightButton->constraints.width = RelativeConstraint(0.9);
    rightButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        OskMoveRequested oskEvent(OskEvent::MoveRight);
        app->getGame()->raiseEvent(oskEvent);
    };
    addChild(rightButton);

    TextButton* forwardButton = new TextButton("controls_menu.move_forward", gui, colors::anthraziteGrey, "Forwards");
    forwardButton->constraints.height = AbsoluteConstraint(30);
    forwardButton->constraints.width = RelativeConstraint(0.9);
    forwardButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        OskMoveRequested oskEvent(OskEvent::MoveForward);
        app->getGame()->raiseEvent(oskEvent);
    };
    addChild(forwardButton);

    TextButton* backwardButton = new TextButton("controls_menu.move_backward", gui, colors::anthraziteGrey, "Backwards");
    backwardButton->constraints.height = AbsoluteConstraint(30);
    backwardButton->constraints.width = RelativeConstraint(0.9);
    backwardButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        OskMoveRequested oskEvent(OskEvent::MoveBackward);
        app->getGame()->raiseEvent(oskEvent);
    };
    addChild(backwardButton);

    TextButton* interactButton = new TextButton("controls_menu.interact", gui, colors::anthraziteGrey, "Interact");
    interactButton->constraints.height = AbsoluteConstraint(30);
    interactButton->constraints.width = RelativeConstraint(0.9);
    interactButton->onClick += [&](const MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        OskMoveRequested oskEvent(OskEvent::Interact);
        app->getGame()->raiseEvent(oskEvent);
    };
    addChild(interactButton);

    // by default
    show();
}