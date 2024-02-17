#pragma once
#include "gui/components/widget.hpp"

#include <stack>

class Application;

class PauseMenu;
class OptionsMenu;
class DebugPanel;
class ControlsPanel;
class LevelSelectMenu;
class Label;

struct KeyEvent;
struct MouseButtonEvent;
struct MouseMoveEvent;

enum class GameMenus {
    NONE,
    MAIN_MENU,
    PAUSE_MENU,
    OPTIONS_MENU,
    LEVEL_SELECT_MENU,
};

class Gui {
  private:
    Application* app;

    float width, height;

    PauseMenu* pauseMenu;
    OptionsMenu* optionsMenu;
    LevelSelectMenu* levelSelectMenu;
    ControlsPanel* controlsPanel;

    DebugPanel* debugPanel;
    Label* warningWidget;

    std::stack<Widget*> navigation;
    std::vector<Widget*> widgets;

    void init();

  public:
    Gui(Application* app, float width, float height);

    void showMenu(GameMenus gameMenu);
    void popMenu();

    void showWarning(const std::string& text) const;
    void hideWarning() const;

    Application* getApp() const;

    void setScreenSize(float width, float height);
    Rectangle getBox() const;

    void update();

    void render() const;

    void handleMouseButtonEvent(MouseButtonEvent& e);
    void handleKeyEvent(KeyEvent& e);
    void handleMouseMoveEvent(MouseMoveEvent& e);
};
