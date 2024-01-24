#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

#include "testgl/chunkpos.hpp"

typedef unsigned char Sides;

enum Side : Sides
{
    NONE = 0,
    FRONT = 1,   // 0
    BACK = 2,    // 1
    LEFT = 4,    // 2
    RIGHT = 8,   // 3
    TOP = 16,    // 4
    BOTTOM = 32, // 5
    ALL = 63
};

namespace CubeMeshSides
{

    const int values_per_face = 18;

    const float faces_array[6][values_per_face] = {
        {
            // positions : front
            -0.5f, -0.5f, 0.5f, //
            0.5f, -0.5f, 0.5f,  //
            0.5f, 0.5f, 0.5f,   //
            0.5f, 0.5f, 0.5f,   //
            -0.5f, 0.5f, 0.5f,  //
            -0.5f, -0.5f, 0.5f, //
        },
        {
            // positions : back
            -0.5f, -0.5f, -0.5f, //
            0.5f, 0.5f, -0.5f,   //
            0.5f, -0.5f, -0.5f,  //
            0.5f, 0.5f, -0.5f,   //
            -0.5f, -0.5f, -0.5f, //
            -0.5f, 0.5f, -0.5f,  //
        },
        {
            // positions : left
            -0.5f, 0.5f, 0.5f,   //
            -0.5f, 0.5f, -0.5f,  //
            -0.5f, -0.5f, -0.5f, //
            -0.5f, -0.5f, -0.5f, //
            -0.5f, -0.5f, 0.5f,  //
            -0.5f, 0.5f, 0.5f,   //
        },
        {
            // positions : right
            0.5f, 0.5f, 0.5f,   //
            0.5f, -0.5f, -0.5f, //
            0.5f, 0.5f, -0.5f,  //
            0.5f, -0.5f, -0.5f, //
            0.5f, 0.5f, 0.5f,   //
            0.5f, -0.5f, 0.5f,  //
        },
        {
            // positions : top
            -0.5f, 0.5f, -0.5f, //
            0.5f, 0.5f, 0.5f,   //
            0.5f, 0.5f, -0.5f,  //
            0.5f, 0.5f, 0.5f,   //
            -0.5f, 0.5f, -0.5f, //
            -0.5f, 0.5f, 0.5f,  //
        },
        {
            // positions : bottom
            -0.5f, -0.5f, -0.5f, //
            0.5f, -0.5f, -0.5f,  //
            0.5f, -0.5f, 0.5f,   //
            0.5f, -0.5f, 0.5f,   //
            -0.5f, -0.5f, 0.5f,  //
            -0.5f, -0.5f, -0.5f, //
        }};
    const float faces_array_normals[6][3] = {
        {0.0f, 0.0f, 1.0f},  // front
        {0.0f, 0.0f, -1.0f}, // back
        {-1.0f, 0.0f, 0.0f}, // left
        {1.0f, 0.0f, 0.0f},  // right
        {0.0f, 1.0f, 0.0f},  // top
        {0.0f, -1.0f, 0.0f}  // bottom
    };

    float *faces_at(float x, float y, float z, Sides sides, float destination[]);
    float *faces_at(glm::vec3 pos, Sides sides, float destination[]);
    float *normals_on(Sides sides, float destination[3 * 6]);
} // namespace CubeMeshSides

Side opposite_side(Side side);
int side_to_index(Side side);
ChunkPos dirFromSide(Side side);