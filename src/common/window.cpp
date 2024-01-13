#include "testgl/window.hpp"
#include "testgl/logging.hpp"
#include "testgl/constants.hpp"
#include "testgl/callbacks.hpp"

Window::Window(const char *title, int height, int width, bool shouldInitGLAD)
    : failed(false)
{
    // Create a window
    log_debug("Creating window");
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        log_fatal("Failed to create window");
        glfwTerminate();
        failed = true;
    }

    // Make the window's context current
    log_debug("Making window context current");
    glfwMakeContextCurrent(window);

    // Setup the GLFW window size callback
    log_debug("Setting up GLFW window size callback");
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwSwapInterval(GLFW_VSYNC); // Enable vsync

    if (shouldInitGLAD)
    {
        initGLAD();
    }

    glfw_window_size_callback(window, width, height);
}

Window::~Window()
{
    // Terminate GLFW
    log_debug("Terminating GLFW");
    glfwTerminate();
}

int Window::initGLFW()
{ // Setup the GLFW error callback
    log_debug("Setting up GLFW error callback");
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    log_debug("Initializing GLFW");
    if (!glfwInit())
    {
        log_fatal("Failed to initialize GLFW");
        return EXIT_FAILURE;
    }

    // Configure GLFW
    log_debug("Configuring GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS and Forward Compatibility

    return EXIT_SUCCESS;
}

int Window::initGLAD()
{
    // Initialize glad
    log_debug("Initializing glad");
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log_fatal("Failed to initialize glad");
        return EXIT_FAILURE;
    }

    // Print OpenGL information
    log_info("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    return EXIT_SUCCESS;
}

bool Window::shouldClose()
{
    return failed || glfwWindowShouldClose(window);
}

void Window::close()
{
    glfwSetWindowShouldClose(window, true);
}