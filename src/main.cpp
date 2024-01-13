#include "testgl/window.hpp"
#include "testgl/main.hpp"
#include "testgl/logging.hpp"
#include "testgl/constants.hpp"
#include "learnopengl/Shaders.hpp"
#include "testgl/player.hpp"
#include "testgl/callbacks.hpp"

int main(int argc, char **argv)
{
    Window::initGLFW();
    Window window("TestGL");

    // Load the base shaders
    log_debug("Loading shaders");
    Shader shader("../shaders/base.vert", "../shaders/base.frag");

    // Load the player
    log_debug("Loading player");
    Player player;

    setup_player_callbacks(window, &player);

    // Add a simple 3D triangle
    log_debug("Adding triangle");
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top
    };
    uint indices[] = {
        0, 1, 2 // first triangle
    };

    uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // Generate 1 buffer
    glGenBuffers(1, &EBO); // Generate 1 buffer

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    // Copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy our indices array in a buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Then set the vertex attributes pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO
    glBindVertexArray(0);

    // Model and inverse model matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 inv_model = glm::inverse(model);

    // Set the model and inverse model matrices
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("inv_model", inv_model);

    // Loop until the user closes the window
    log_debug("Starting main loop");
    while (!window.shouldClose())
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        player.processKeyboard(window, 1 / 60.0f);
        player.setupCameraTransform(&shader, WINDOW_WIDTH, WINDOW_HEIGHT);

        // Draw the triangle
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}