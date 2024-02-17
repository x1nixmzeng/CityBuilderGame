#pragma once
#include <vector>

#include "game.hpp"
#include "gui/gui.hpp"

#include <raylib.h>

class System;
struct KeyEvent;
struct FramebufferSizeEvent;
struct MouseButtonEvent;
struct MouseMoveEvent;
struct MouseScrollEvent;

//static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//static void cursorPos_callback(GLFWwindow* window, double x, double y);
//static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class Application {
  private:
    
    Gui* gui = nullptr;
    Game* game = nullptr;

    bool stopRequested = false;

    //friend void cursorPos_callback(GLFWwindow*, double, double);
    Vector2 lastCursorPos = Vector2(400.0f, 300.0f);

    void init();

  public:
    float updateTime = 0.0f;

    Application();

    void run();

    void stop();

    inline Game* getGame() const {
        return game;
    }

    inline void setGameState(GameState state) {
        game->setState(state);
    }

    inline const Gui* getGui() const {
        return gui;
    }

    inline GameState getGameState() const {
        return game->getState();
    }

    void onKeyEvent(KeyEvent& e);
    void onFramebufferSizeEvent(FramebufferSizeEvent& e);
    void onMouseMoveEvent(MouseMoveEvent& e);
    void onMouseButtonEvent(MouseButtonEvent& e);
    void onMouseScrollEvent(MouseScrollEvent& e);
};