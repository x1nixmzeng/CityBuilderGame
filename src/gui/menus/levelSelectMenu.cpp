#include "gui/menus/levelSelectMenu.hpp"

#include "events/laraEvents.hpp"
#include "events/mouseEvents.hpp"

#include "application.hpp"

#include "gui/components/button.hpp"

#include <format>

LevelSelectMenu::LevelSelectMenu(Gui* gui)
    : StackPanel("level_select_menu", gui, StackOrientation::COLUMN, colors::transparent) {
    constraints.width = RelativeConstraint(0.6f);
    constraints.height = AbsoluteConstraint(120.0f);

    for (int level = 1; level < 6; ++level) {
        std::string levelName = std::format("level_{}", level);
        std::string levelId = std::format("level_select_menu.{}", levelName);
        std::string displayName = std::format("Level {}", level);

        TextButton* level_button = new TextButton(levelId, gui, colors::anthraziteGrey, displayName);
        level_button->constraints.height = RelativeConstraint(0.5f);
        level_button->constraints.width = RelativeConstraint(0.5f);
        level_button->onClick += [=](MouseButtonEvent& e) {
            Application* app = this->gui->getApp();
            RequestLevelEvent level{levelName};
            app->getGame()->raiseEvent(level);

            this->gui->popMenu();
            this->gui->popMenu();
            this->gui->popMenu();
            e.handled = true;

            MouseMoveEvent fakeEvent{FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
            level_button->onMouseLeave.invoke(fakeEvent);
        };
        level_button->cornerRadius = 15.0f;
        addChild(level_button);
    }
}
