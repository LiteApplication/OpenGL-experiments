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

    // Capture the mouse if needed
    if (CAPTURE_MOUSE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (shouldInitGLAD)
    {
        initGLAD();
    }
    glfw_window_size_callback(window, width, height);
    initGL();
}

Window::~Window()
{
    // Terminate GLFW
    log_debug("Terminating GLFW");
    glfwTerminate();
}

void Window::initGL()
{

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        log_debug("Enabling OpenGL debug context");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
    }
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");

    glClearColor(0.1f, 0.3f, 0.75f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
#ifdef GLFW_REQUEST_DEBUG_CONTEXT
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // Debug context
#endif
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