#include "application.hpp"

#include "modelLoader.hpp"

#include "systems/cameraSystem.hpp"
#include "systems/renderSystem.hpp"
#include "systems/system.hpp"

#include "rendering/geometry.hpp"
#include "rendering/shader.hpp"

#include "components/components.hpp"

#include "events/framebufferSizeEvent.hpp"
#include "events/keyEvent.hpp"
#include "events/mouseEvents.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->onFramebufferSizeEvent(FramebufferSizeEvent{app, width, height});
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->onKeyEvent(KeyEvent{app, key, scancode, action, mods});
}

void cursorPos_callback(GLFWwindow* window, double x, double y) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->onMouseMoveEvent(MouseMoveEvent{app, (float)x, (float)y, app->lastCursorPos.x, app->lastCursorPos.y});

    app->lastCursorPos = glm::vec2(x, y);
}

void mouseButton_callback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    app->onMouseButtonEvent(MouseButtonEvent{app, (float)x, (float)y, button, action, mods});
}

void Application::init() {
    if (!glfwInit()) {
        std::cerr << "failed to intialize GLFW!" << std::endl;
        exit(1);
    }

    window = glfwCreateWindow(800, 600, "Traffic Simulation", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        exit(1);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursorPos_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glewInit();

    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // init gui
    gui = new Gui();
    gui->setScreenSize(800, 600);
    gui->init();

    // init game
    game = new Game(this);
}

Application::Application() {
    init();
}

void Application::run() {
    glfwSetTime(0);
    float lastTime = 0;

    while (!stopRequested) {
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.0f, 0.698f, 0.894f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game->update(dt);

        gui->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        stopRequested |= glfwWindowShouldClose(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Application::getWindow() const {
    return window;
}

void Application::onKeyEvent(const KeyEvent& e) {
    if (e.action == GLFW_PRESS) {
        switch (e.key) {
        case GLFW_KEY_ESCAPE:
            // change gui visibility
            if (gui->visible) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                gui->visible = false;
                game->paused = false;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                gui->visible = true;
                game->paused = true;
            }
            break;
        }
    }

    game->raiseEvent(e);
}

void Application::onFramebufferSizeEvent(const FramebufferSizeEvent& e) {
    gui->setScreenSize(e.width, e.height);

    game->raiseEvent(e);
}

void Application::onMouseMoveEvent(const MouseMoveEvent& e) {
    game->raiseEvent(e);
}

void Application::onMouseButtonEvent(const MouseButtonEvent& e) {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
        if (gui->visible) {
            const GuiElement* element = gui->getElement(e.x, e.y);

            if (element->id == "mainMenu_saveExit") {
                // stop the application
                stopRequested = true;
            }
            else if (element->id == "mainMenu_continue") {
                // close gui
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                gui->visible = false;
                game->paused = false;
            }
        }
    }

    game->raiseEvent(e);
}
