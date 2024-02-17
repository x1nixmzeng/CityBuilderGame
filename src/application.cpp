
#include "application.hpp"

#include "systems/system.hpp"

#include "components/components.hpp"

#include "events/framebufferSizeEvent.hpp"
#include "events/keyEvent.hpp"
#include "events/mouseEvents.hpp"

#include <iostream>

// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//
//     // todo: ensure this is never 0
//
//     glViewport(0, 0, width, height);
//
//     Application* app = (Application*)glfwGetWindowUserPointer(window);
//     FramebufferSizeEvent event = FramebufferSizeEvent(width, height);
//     app->onFramebufferSizeEvent(event);
// }
//
// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//     Application* app = (Application*)glfwGetWindowUserPointer(window);
//     KeyEvent event = KeyEvent(key, scancode, action, mods);
//     app->onKeyEvent(event);
// }
//
// void cursorPos_callback(GLFWwindow* window, double x, double y) {
//     Application* app = (Application*)glfwGetWindowUserPointer(window);
//     MouseMoveEvent event = MouseMoveEvent{(float)x, (float)y, app->lastCursorPos.x, app->lastCursorPos.y};
//     app->onMouseMoveEvent(event);
//
//     app->lastCursorPos = glm::vec2(x, y);
// }
//
// void mouseButton_callback(GLFWwindow* window, int button, int action, int mods) {
//     Application* app = (Application*)glfwGetWindowUserPointer(window);
//     double x, y;
//     glfwGetCursorPos(window, &x, &y);
//
//     MouseButtonEvent event = MouseButtonEvent{(float)x, (float)y, button, action, mods};
//     app->onMouseButtonEvent(event);
// }
//
// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//     Application* app = (Application*)glfwGetWindowUserPointer(window);
//
//     MouseScrollEvent event = MouseScrollEvent((float)xoffset, (float)yoffset);
//     app->onMouseScrollEvent(event);
// }

void Application::init() {

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "city builder demo");

    SetExitKey(0);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // DisableCursor(); // Limit cursor to relative movement inside the window
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // init gui
    gui = new Gui(this, screenWidth, screenHeight);

    // init game
    game = new Game(this);
}

Application::Application() {
    init();
}

void Application::run() {

    float lastTime = 0;

    // Main game loop
    while (!stopRequested) // Detect window close button or ESC key
    {
        // float currentTime = (float)glfwGetTime();
        updateTime = 1.0 / 60.0; // currentTime - lastTime;
        // lastTime = currentTime;

        // this is super gross - we don't support callbacks?? vvvvvvvvvvvvvvvvvvv

        if (IsKeyDown(KEY_ESCAPE)) {
            KeyEvent event = KeyEvent(KEY_ESCAPE, 0, KeyAction::Press, 0);
            onKeyEvent(event);
        }

        auto md = GetMouseDelta();
        if (md.x != 0.0f || md.y != 0.0f) {
            auto cursorPos = GetMousePosition();

            MouseMoveEvent event = MouseMoveEvent(cursorPos.x, cursorPos.y, lastCursorPos.x, lastCursorPos.y);
            onMouseMoveEvent(event);

            lastCursorPos = cursorPos;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            MouseButtonEvent event = MouseButtonEvent{lastCursorPos.x, lastCursorPos.y, MOUSE_BUTTON_LEFT, 0, 0};
            onMouseButtonEvent(event);
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            MouseButtonEvent event = MouseButtonEvent{lastCursorPos.x, lastCursorPos.y, MOUSE_BUTTON_LEFT, 1, 0};
            onMouseButtonEvent(event);
        }

        
        // raylib doesn't support callbacks. ugly workaround
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // imgui background colours
        ClearBackground(ColorFromNormalized(Vector4(0.45f, 0.55f, 0.60f, 1.00f)));

        game->update(updateTime);

        gui->update();
        gui->render();

        EndDrawing();

        stopRequested |= WindowShouldClose();
    }

    CloseWindow();
}

void Application::stop() {
    stopRequested = true;
}

void Application::onKeyEvent(KeyEvent& e) {
    gui->handleKeyEvent(e);

    if (!e.handled) {
        game->raiseEvent(e);
    }
}

void Application::onFramebufferSizeEvent(FramebufferSizeEvent& e) {
    gui->setScreenSize(e.width, e.height);

    game->raiseEvent(e);
}

void Application::onMouseMoveEvent(MouseMoveEvent& e) {
    game->raiseEvent(e);

    gui->handleMouseMoveEvent(e);
}

void Application::onMouseButtonEvent(MouseButtonEvent& e) {
    gui->handleMouseButtonEvent(e);

    if (!e.handled) {
        game->raiseEvent(e);
    }
}

void Application::onMouseScrollEvent(MouseScrollEvent& e) {
    game->raiseEvent(e);
}
