#include "testgl/cube.hpp"
#include "testgl/logging.hpp"

#include <glm/glm.hpp>
#include <cstring>
#include <vector>

namespace CubeMeshSides
{
    float *faces_at(float x, float y, float z, Sides sides, float destination[])
    {
        // Count the number of 1 bits in sides
        int current = 0;
        for (int f = 0; f < 6; f++)
        {
            if (sides & (1 << f))
            {
                for (int i = 0; i < values_per_face;)
                {
                    destination[current++] = faces_array[f][i++] + x;
                    destination[current++] = faces_array[f][i++] + y;
                    destination[current++] = faces_array[f][i++] + z;
                }
            }
        }
        return &destination[current];
    }

    float *faces_at(glm::vec3 pos, Sides side, float destination[])
    {
        return faces_at(pos.x, pos.y, pos.z, side, destination);
    }

    float *normals_on(Sides sides, float destination[3 * 6])
    {
        int current = 0;
        for (int f = 0; f < 6; f++)
        {
            if (sides & (1 << f))
            {
                for (int v = 0; v < 6; v++)
                    for (int i = 0; i < 3; i++)
                    {
                        destination[current] = faces_array_normals[f][i];
                        current++;
                    }
            }
        }
        return &destination[current];
    }
}

Side opposite_side(Side side)
{
    switch (side)
    {
    case Side::FRONT:
        return Side::BACK;
    case Side::BACK:
        return Side::FRONT;
    case Side::LEFT:
        return Side::RIGHT;
    case Side::RIGHT:
        return Side::LEFT;
    case Side::TOP:
        return Side::BOTTOM;
    case Side::BOTTOM:
        return Side::TOP;
    default:
        return Side::NONE;
    }
}

int side_to_index(Side side)
{
    switch (side)
    {
    case Side::FRONT:
        return 0;
    case Side::BACK:
        return 1;
    case Side::LEFT:
        return 2;
    case Side::RIGHT:
        return 3;
    case Side::TOP:
        return 4;
    case Side::BOTTOM:
        return 5;
    default:
        return -1;
    }
}

ChunkPos dirFromSide(Side side)
{
    switch (side)
    {
    case Side::FRONT:
        return ChunkPos(0, 0, 1);
    case Side::BACK:
        return ChunkPos(0, 0, -1);
    case Side::LEFT:
        return ChunkPos(-1, 0, 0);
    case Side::RIGHT:
        return ChunkPos(1, 0, 0);
    case Side::TOP:
        return ChunkPos(0, 1, 0);
    case Side::BOTTOM:
        return ChunkPos(0, -1, 0);
    default:
        return ChunkPos(0, 0, 0);
    }
}