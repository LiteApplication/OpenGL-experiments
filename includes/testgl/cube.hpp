#pragma once

#include <glm/glm.hpp>
#include <vector>

typedef unsigned char Sides;
enum Side : Sides
{
    NONE = 0,
    FRONT = 1,
    BACK = 2,
    LEFT = 4,
    RIGHT = 8,
    TOP = 16,
    BOTTOM = 32,
    ALL = 63
};

namespace CubeMeshSides
{
    const float face_front[] = {
        // positions
        -0.5f, -0.5f, 0.5f, //
        0.5f, -0.5f, 0.5f,  //
        0.5f, 0.5f, 0.5f,   //
        0.5f, 0.5f, 0.5f,   //
        -0.5f, 0.5f, 0.5f,  //
        -0.5f, -0.5f, 0.5f, //
    };

    const float face_back[] = {
        // positions
        -0.5f, -0.5f, -0.5f, //
        0.5f, 0.5f, -0.5f,   //
        0.5f, -0.5f, -0.5f,  //
        0.5f, 0.5f, -0.5f,   //
        -0.5f, -0.5f, -0.5f, //
        -0.5f, 0.5f, -0.5f,  //
    };

    const float face_left[] = {
        // positions
        -0.5f, 0.5f, 0.5f,   //
        -0.5f, 0.5f, -0.5f,  //
        -0.5f, -0.5f, -0.5f, //
        -0.5f, -0.5f, -0.5f, //
        -0.5f, -0.5f, 0.5f,  //
        -0.5f, 0.5f, 0.5f,   //
    };

    const float face_right[] = {
        // positions
        0.5f, 0.5f, 0.5f,   //
        0.5f, -0.5f, -0.5f, //
        0.5f, 0.5f, -0.5f,  //
        0.5f, -0.5f, -0.5f, //
        0.5f, 0.5f, 0.5f,   //
        0.5f, -0.5f, 0.5f,  //
    };

    const float face_top[] = {
        // positions
        -0.5f, 0.5f, -0.5f, //
        0.5f, 0.5f, 0.5f,   //
        0.5f, 0.5f, -0.5f,  //
        0.5f, 0.5f, 0.5f,   //
        -0.5f, 0.5f, -0.5f, //
        -0.5f, 0.5f, 0.5f,  //
    };

    const float face_bottom[] = {
        // positions
        -0.5f, -0.5f, -0.5f, //
        0.5f, -0.5f, -0.5f,  //
        0.5f, -0.5f, 0.5f,   //
        0.5f, -0.5f, 0.5f,   //
        -0.5f, -0.5f, 0.5f,  //
        -0.5f, -0.5f, -0.5f, //
    };

    const glm::vec3 face_front_normal = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 face_back_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 face_left_normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    const glm::vec3 face_right_normal = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 face_top_normal = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 face_bottom_normal = glm::vec3(0.0f, -1.0f, 0.0f);

    std::vector<float> faces_on(Sides sides);

    std::vector<float> faces_at(float x, float y, float z, Sides sides);
    std::vector<float> faces_at(glm::vec3 pos, Sides sides);
    std::vector<glm::vec3> normals_on(Sides sides);
} // namespace CubeMeshSides

Side opposite_side(Side side);