#ifndef CAMERA_H
#define CAMERA_H

#include "testgl/logging.hpp"
// #include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum struct CameraMovement : u_char
{
    NONE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 4,
    RIGHT = 8,
    BOOST = 16,
    DOUBLE_BOOST = 32,
    UP = 64,
    DOWN = 128
};

// Default camera values
const float CAMERA_YAW = -90.0f;
const float CAMERA_PITCH = 0.0f;
const float PLAYER_SPEED = 16.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
    uint screen_width;
    uint screen_height;
    float near_plane;
    float far_plane;
    bool fov_changed;
    glm::mat4 projection;

    // camera Attributes
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float fov;

public:
    // The only attribute that are useful for the world to know about
    glm::vec3 Position;
    glm::vec3 Front;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = CAMERA_YAW,
           float pitch = CAMERA_PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(PLAYER_SPEED),
          MouseSensitivity(SENSITIVITY), fov(ZOOM), screen_height(0),
          screen_width(0), near_plane(0), far_plane(0), fov_changed(true)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        fov_changed = true;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(PLAYER_SPEED),
          MouseSensitivity(SENSITIVITY), fov(ZOOM), screen_height(0),
          screen_width(0), near_plane(0), far_plane(0), fov_changed(true)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 GetProjectionMatrix(uint width, uint height, float near,
                                  float far)
    {
        if (width != screen_width || height != screen_height ||
            near != near_plane || far != far_plane || fov_changed)
        {
            screen_width = width;
            screen_height = height;
            near_plane = near;
            far_plane = far;
            fov_changed = false;
            projection = glm::perspective(glm::radians(fov),
                                          (float)width / (float)height, near, far);
            log_debug("Projection matrix updated");
        } // reduce the number of calls to glm::perspective
        return projection;
    }

    // processes input received from any keyboard-like input system. Accepts input
    // parameter in the form of camera defined ENUM (to abstract it from windowing
    // systems)
    void ProcessKeyboard(u_char direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 movement(0.0f);
        glm::vec3 front = Front;
        front.y = 0.0f; // Constraint the camera to the xz plane
        front = glm::normalize(front);
        if (direction & (u_char)CameraMovement::BOOST)
            velocity *= 4;
        if (direction & (u_char)CameraMovement::DOUBLE_BOOST)
            velocity *= 64;
        if (direction & (u_char)CameraMovement::FORWARD)
            movement += front * velocity;
        if (direction & (u_char)CameraMovement::BACKWARD)
            movement -= front * velocity;
        if (direction & (u_char)CameraMovement::LEFT)
            movement -= Right * velocity;
        if (direction & (u_char)CameraMovement::RIGHT)
            movement += Right * velocity;
        if (direction & (u_char)CameraMovement::UP)
            movement.y += velocity;
        if (direction & (u_char)CameraMovement::DOWN)
            movement.y -= velocity;
        Position += movement;

        updateCameraVectors();
    }

    // processes input received from a mouse input system. Expects the offset
    // value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset,
                              bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires
    // input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        fov_changed = true;
        fov -= (float)yoffset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 90.0f)
            fov = 90.0f;

        // reduce the speed of the mouse with fov change
        MouseSensitivity = SENSITIVITY * fov / ZOOM;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets
        // closer to 0 the more you look up or down which
        // results in slower movement.
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif