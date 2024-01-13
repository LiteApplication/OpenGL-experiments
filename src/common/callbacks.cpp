#include "testgl/player.hpp"
#include "testgl/callbacks.hpp"
#include "testgl/logging.hpp"

// glfw error callback
void glfw_error_callback(int error, const char *description)
{
    log_error("GLFW Error (%d): %s", error, description);
}

void glfw_window_size_callback(GLFWwindow *window, int width, int height)
{
    log_debug("Window resized to %dx%d", width, height);
    glViewport(0, 0, width, height);
}

void setup_player_callbacks(GLFWwindow *window, Player *player)
{
    // Set the player as the user pointer for the window
    glfwSetWindowUserPointer(window, player);

    // Setup the GLFW key callback
    log_debug("Setting up GLFW key callback");
    glfwSetKeyCallback(window, glfw_key_callback);

    // Setup the GLFW mouse button callback
    log_debug("Setting up GLFW mouse button callback");
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);

    // Setup the GLFW mouse position callback
    log_debug("Setting up GLFW mouse position callback");
    glfwSetCursorPosCallback(window, glfw_mouse_position_callback);

    // Setup the GLFW mouse scroll callback
    log_debug("Setting up GLFW mouse scroll callback");
    glfwSetScrollCallback(window, glfw_mouse_scroll_callback);
}

// glfw key callback
void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Close the window when the user presses escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        log_debug("Escape key pressed, closing window");
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// glfw mouse button callback
void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    Player *player = get_player(window);
    player->mouse_button_callback(button, action, mods);
}

// glfw mouse position callback
void glfw_mouse_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    Player *player = get_player(window);
    player->mouse_callback(xpos, ypos);
}

// glfw mouse scroll callback
void glfw_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Player *player = get_player(window);
    player->scroll_callback(yoffset);
}

Player *get_player(GLFWwindow *window)
{
    return (Player *)glfwGetWindowUserPointer(window);
}