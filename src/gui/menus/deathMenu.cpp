#include "gui/menus/deathMenu.hpp"

#include "events/laraEvents.hpp"
#include "events/mouseEvents.hpp"

#include "gui/components/button.hpp"

#include "application.hpp"

DeathMenu::DeathMenu(Gui* gui)
    : StackPanel("death_menu", gui, StackOrientation::COLUMN, colors::transparent) {

    constraints.width = RelativeConstraint(0.6f);
    constraints.height = AbsoluteConstraint(195.0f);

    TextButton* retry = new TextButton("deathMenu_retry", gui, colors::anthraziteGrey, "Retry");
    retry->constraints.height = AbsoluteConstraint(45.0f);
    retry->constraints.width = RelativeConstraint(0.9f);
    retry->onClick += [&](MouseButtonEvent& e) {
        Application* app = this->gui->getApp();
        RequestLevelRestart levelRestart;
        app->getGame()->raiseEvent(levelRestart);

        this->gui->popMenu();

        //MouseMoveEvent fakeEvent{FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
        //level_button->onMouseLeave.invoke(fakeEvent);
    };
    retry->cornerRadius = 15.0f;
    addChild(retry);

    TextButton* saveAndExit = new TextButton("deathMenu_saveExit", gui, colors::anthraziteGrey, "Close Game");
    saveAndExit->constraints.height = AbsoluteConstraint(45.0f);
    saveAndExit->constraints.width = RelativeConstraint(0.9f);
    saveAndExit->onClick += [&](const MouseButtonEvent& e) {
        this->gui->getApp()->stop();
    };
    saveAndExit->cornerRadius = 15.0f;
    addChild(saveAndExit);
}
