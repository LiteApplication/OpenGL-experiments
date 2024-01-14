#include "testgl/window.hpp"
#include "testgl/main.hpp"
#include "testgl/logging.hpp"
#include "testgl/constants.hpp"
#include "learnopengl/Shaders.hpp"
#include "testgl/player.hpp"
#include "testgl/callbacks.hpp"
#include "testgl/chunk.hpp"
#include "testgl/world.hpp"

#include <chrono>

int main(int argc, char **argv)
{
    if (Window::initGLFW() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    Window window("TestGL");

    // Load the base shaders
    log_debug("Loading shaders");
    Shader shader("../shaders/base.vert", "../shaders/base.frag");

    // Load the player
    log_debug("Loading player");
    Player player;

    setup_player_callbacks(window, &player);

    // Create the world
    log_debug("Creating world");
    World world(player.getPositionPtr(), WorldGenerator::flat);

    // Loop until the user closes the window
    log_debug("Starting main loop");
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose())
    {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
        previousTime = currentTime;

        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        player.processKeyboard(window, deltaTime);
        player.setupCameraTransform(&shader, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (player.debugMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        world.tick();
        world.graphicalTick(&shader);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}