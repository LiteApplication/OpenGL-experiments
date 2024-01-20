#include "testgl/player.hpp"
Player::Player(glm::vec3 position) : first_mouse(true), camera(position, glm::vec3(0.0f, 1.0f, 0.0f)), debugMode(false)
{
}

void Player::mouse_callback(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    // reversed since y-coordinates go from bottom to top
    float yoffset = last_y - ypos;

    // Cancel the mouse movement if it's too strong
    if (abs(xoffset) > 100.0f || abs(yoffset) > 100.0f)
    {
        xoffset = 0.0f;
        yoffset = 0.0f;
    }

    last_x = xpos;
    last_y = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void Player::scroll_callback(float yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void Player::processKeyboard(GLFWwindow *window, float deltaTime)
{
    u_char direction((u_char)CameraMovement::NONE);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::FORWARD;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::BACKWARD;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::LEFT;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::RIGHT;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::UP;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::DOWN;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::BOOST;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        direction = direction | (u_char)CameraMovement::DOUBLE_BOOST;
    if (direction != (u_char)CameraMovement::NONE)
        camera.ProcessKeyboard(direction, deltaTime);
}

void Player::setupCameraTransform(Shader *shader, uint screen_w,
                                  uint screen_h)
{
    shader->use();
    shader->setMat4("view", camera.GetViewMatrix());
    shader->setMat4("projection", camera.GetProjectionMatrix(
                                      screen_w, screen_h, 0.1f,
                                      // We use +3 because it would be 1 + sqrt(2) but no need for useless computation
                                      (VIEW_DISTANCE + 3) * CHUNK_SIZE)); // View distance here
}

void Player::mouse_button_callback(int button, int action, int mods)
{
    // Place a block if the left mouse button is pressed
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glm::vec3 block_pos = camera.Position;
    }
    // Delete the block if the right mouse button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glm::vec3 block_pos = camera.Position;
    }
}

Player::~Player()
{
}
