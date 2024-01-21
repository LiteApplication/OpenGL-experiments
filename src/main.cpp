#include "testgl/window.hpp"
#include "testgl/main.hpp"
#include "testgl/logging.hpp"
#include "learnopengl/Shaders.hpp"

#include <chrono>
#include <thread>

int main(int argc, char **argv)
{
    if (Window::initGLFW() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    Window window(WINDOW_TITLE);

    // Load the base shaders
    log_debug("Loading shaders");
    Shader shader("../shaders/base.vert", "../shaders/base.frag"); // Relative to the build directory

    // Load the player
    log_debug("Loading player");
    Player player;

    setup_player_callbacks(window, &player);

    // Create the world
    log_debug("Creating world");
    World world(player.getPositionPtr(), WorldGenerator::perlin);
    if (GEN_ALL_CHUNKS_ON_START)
        world.loadAllChunks();

    // Create the tick thread
    log_debug("Creating tick thread");
    std::thread tickThread(tick_thread, &world, &player, &window);

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

        world.graphicalTick(&shader);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Check if the tick thread is still running
    if (tickThread.joinable())
    {
        log_info("Waiting for tick thread to exit");
        tickThread.join();
    }

    return EXIT_SUCCESS;
}

// Separate function to run ticks on another thread
void tick_thread(World *world, Player *player, Window *window)
{
    log_info("Tick thread started");
    auto previousTime = std::chrono::system_clock::now();
    while (!window->shouldClose())
    {
        // Calculate delta time
        auto currentTime = std::chrono::system_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
        previousTime = currentTime;

        // Wait for the next tick
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / TICKS_PER_SECOND - (int)(deltaTime * 1000)));

        world->tick();
    }

    log_info("Tick thread exiting");
}
