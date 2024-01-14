#pragma once

#include "learnopengl/Shaders.hpp"
#include "learnopengl/Camera.hpp"
#include "testgl/constants.hpp"
#include "testgl/logging.hpp"

#include <GLFW/glfw3.h>

class Player
{
private:
    Camera camera;
    bool first_mouse;
    float last_x, last_y;

public:
    bool debugMode;
    Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    ~Player();

    void processKeyboard(GLFWwindow *window, float deltaTime);
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(float yoffset);
    void mouse_button_callback(int button, int action, int mods);

    void setupCameraTransform(Shader *shader, uint screen_w,
                              uint screen_h);
    void toggle_debug() { debugMode = !debugMode; }
    glm::vec3 *getPositionPtr() { return &camera.Position; }
};
