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

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    const char *source_str;
    const char *type_str;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        source_str = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "Other";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "Other";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        log_error("OpenGL: Source : %s, Type : %s, ID : %d: %s", source_str, type_str, id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        log_warn("OpenGL: Source : %s, Type : %s, ID : %d: %s", source_str, type_str, id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        log_info("OpenGL: Source : %s, Type : %s, ID : %d: %s", source_str, type_str, id, message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        log_debug("OpenGL: Source : %s, Type : %s, ID : %d: %s", source_str, type_str, id, message);
        break;
    }
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

    // Pressed F3
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
    {
        log_debug("F3 key pressed, toggling debug mode");
        Player *player = get_player(window);
        player->toggle_debug();
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