#pragma once
#include "gui/components/widget.hpp"

#include "gui/menus.hpp"
#include <stack>

class Application;

class PauseMenu;
class OptionsMenu;
class DebugPanel;
class ControlsPanel;
class InspectorPanel;
class LevelSelectMenu;
class DeathMenu;
class Label;

struct KeyEvent;
struct MouseButtonEvent;
struct MouseMoveEvent;
struct OnLaraDiedEvent;

class Gui {
  private:
    Application* app;

    float width, height;

    PauseMenu* pauseMenu;
    OptionsMenu* optionsMenu;
    LevelSelectMenu* levelSelectMenu;
    ControlsPanel* controlsPanel;
    InspectorPanel* inspectorPanel;
    DeathMenu* deathMenu;

    DebugPanel* debugPanel;
    Label* warningWidget;

    std::stack<Widget*> navigation;
    std::vector<Widget*> allWidgets;
    std::vector<Widget*> panels;

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
    void handleOnLaraDiedEvent(OnLaraDiedEvent& e);
};
