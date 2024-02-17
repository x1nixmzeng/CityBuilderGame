#include "gui/gui.hpp"

#include "gui/components/icon.hpp"
#include "gui/components/label.hpp"
#include "gui/components/stackPanel.hpp"
#include "gui/components/widget.hpp"

#include "gui/menus/controlsPanel.hpp"
#include "gui/menus/deathMenu.hpp"
#include "gui/menus/debugPanel.hpp"
#include "gui/menus/levelSelectMenu.hpp"
#include "gui/menus/optionsMenu.hpp"
#include "gui/menus/pauseMenu.hpp"

#include "events/keyEvent.hpp"
#include "events/mouseEvents.hpp"
#include "events/laraEvents.hpp"

#include "application.hpp"

Gui::Gui(Application* app, float width, float height)
    : app(app), width(width), height(height) {
    init();
}

void Gui::init() {
    pauseMenu = new PauseMenu(this);
    optionsMenu = new OptionsMenu(this);
    levelSelectMenu = new LevelSelectMenu(this);
    controlsPanel = new ControlsPanel(this);
    debugPanel = new DebugPanel(this);
    deathMenu = new DeathMenu(this);
    warningWidget = new Label("warning_label", this, colors::transparent, "", colors::warning);
    warningWidget->hide();

    widgets = {pauseMenu, optionsMenu, debugPanel, levelSelectMenu, controlsPanel, deathMenu};

    // not great. breaks abstraction
    app->getGame()->getEventDispatcher().sink<OnLaraDiedEvent>().connect<&Gui::handleOnLaraDiedEvent>(*this);
}

void Gui::showMenu(GameMenus menu) {
    // hide the menu at the top of the navigation stack
    if (!navigation.empty())
        navigation.top()->hide();

    switch (menu) {
        case GameMenus::NONE:
            // clear the navigation and set game state to running
            while (!navigation.empty()) {
                navigation.pop();
            }

            app->setGameState(GameState::RUNNING);
            return;
        case GameMenus::PAUSE_MENU:
            navigation.push(pauseMenu);
            break;
        case GameMenus::OPTIONS_MENU:
            navigation.push(optionsMenu);
            break;
        case GameMenus::LEVEL_SELECT_MENU:
            navigation.push(levelSelectMenu);
            break;
        case GameMenus::DEATH_MENU:
            navigation.push(deathMenu);
            break;
        default:
            return;
    }

    // show the top menu on the navigation stack and set the game state to paused
    navigation.top()->show();
    app->setGameState(GameState::PAUSED);
}

void Gui::popMenu() {
    if (navigation.empty())
        return;

    // hide the top menu and remove it from the navigation stack
    navigation.top()->hide();
    navigation.pop();

    // if the navigation is not empty show the menu at the top
    if (!navigation.empty()) {
        navigation.top()->show();
    }
    else {
        // if the navigation stack is empty set the game state to running
        app->setGameState(GameState::RUNNING);
    }
}

void Gui::showWarning(const std::string& text) const {
    warningWidget->text = text;
    warningWidget->show();
}

void Gui::hideWarning() const {
    warningWidget->hide();
}

Application* Gui::getApp() const {
    return app;
}

void Gui::setScreenSize(float width, float height) {
    // set new screen size and update the text renderer screen size
    this->width = width;
    this->height = height;

    // update widgets
    for (const auto& widget : widgets) {
        Container* container;
        if ((container = dynamic_cast<Container*>(widget)) != nullptr) {
            container->setChildConstraints();
        }
    }
}

Rectangle Gui::getBox() const {
    return Rectangle{0, 0, width, height};
}

void Gui::update() {
    for (Widget* widget : widgets) {
        widget->update();
    }
}

void Gui::render() const {

    // render top menu
    if (!navigation.empty()) {
        navigation.top()->render();
    }
    controlsPanel->render();

    debugPanel->render();

    warningWidget->render();
}

void Gui::handleMouseButtonEvent(MouseButtonEvent& event) {
    if (!navigation.empty()) {
        navigation.top()->handleMouseButtonEvent(event);
    }

    if (controlsPanel->isVisible()) {
        controlsPanel->handleMouseButtonEvent(event);
    }

    if (debugPanel->isVisible()) {
        debugPanel->handleMouseButtonEvent(event);
    }
}

void Gui::handleKeyEvent(KeyEvent& e) {
    if (e.action == KeyAction::Press) {
        if (app->getGameState() == GameState::RUNNING) {
            switch (e.key) {
                case KEY_ESCAPE:
                    if (!navigation.empty()) {
                        popMenu();
                    }
                    else {
                        showMenu(GameMenus::PAUSE_MENU);
                    }

                    e.handled = true;
                    break;
                case KEY_F1:
                    if (debugPanel->isVisible()) {
                        debugPanel->hide();
                    }
                    else {
                        debugPanel->show();
                    }
                    e.handled = true;
                    break;

                case KEY_F2:
                    if (controlsPanel->isVisible()) {
                        controlsPanel->hide();
                    }
                    else {
                        controlsPanel->show();
                    }
                    e.handled = true;
                    break;
            }
        }
    }
}

void Gui::handleMouseMoveEvent(MouseMoveEvent& event) {

    if (!navigation.empty()) {
        navigation.top()->handleMouseMoveEvent(event);
    }

    if (controlsPanel->isVisible()) {
        controlsPanel->handleMouseMoveEvent(event);
    }

    if (debugPanel->isVisible()) {
        debugPanel->handleMouseMoveEvent(event);
    }
}

void Gui::handleOnLaraDiedEvent(OnLaraDiedEvent& e) {
    assert(app->getGameState() == GameState::RUNNING);

    // Replace pause screen with death menu
    app->setGameState(GameState::PAUSED);
    showMenu(GameMenus::DEATH_MENU);
}
