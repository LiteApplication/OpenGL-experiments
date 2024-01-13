#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "testgl/constants.hpp"

class Window
{
private:
    GLFWwindow *window;

    bool failed;

public:
    Window(const char *title, int height = WINDOW_HEIGHT, int width = WINDOW_WIDTH, bool shouldInitGLAD = true);
    ~Window();

    // Allow the window to be used as a GLFWwindow pointer
    operator GLFWwindow *() const { return window; }

    static int initGLFW();
    static int initGLAD();

    bool shouldClose();
    void close();
};