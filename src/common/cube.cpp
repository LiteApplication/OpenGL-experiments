#include "testgl/cube.hpp"
#include "testgl/logging.hpp"

#include <glm/glm.hpp>
#include <cstring>
#include <vector>

namespace CubeMeshSides
{
    std::vector<float> faces(Sides sides)
    {
        // Count the number of 1 bits in side
        int num_faces = 0;
        for (int i = 0; i < 6; i++)
        {
            if (sides & (1 << i))
                num_faces++;
        }

        std::vector<float> faces_vec;
        faces_vec.reserve(num_faces * sizeof(face_front));

        if (sides & Side::FRONT)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_front),
                             std::end(face_front));
        }
        if (sides & Side::BACK)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_back),
                             std::end(face_back));
        }
        if (sides & Side::LEFT)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_left),
                             std::end(face_left));
        }
        if (sides & Side::RIGHT)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_right),
                             std::end(face_right));
        }
        if (sides & Side::TOP)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_top),
                             std::end(face_top));
        }
        if (sides & Side::BOTTOM)
        {
            faces_vec.insert(faces_vec.end(), std::begin(face_bottom),
                             std::end(face_bottom));
        }

        return faces_vec;
    }

    std::vector<float> faces_at(float x, float y, float z, Sides sides)
    {
        std::vector<float> faces_vec = faces(sides);

        assert(faces_vec.size() % 3 == 0); // Each vertex has 3 coordinates

        for (int i = 0; i < faces_vec.size(); i += 3)
        {
            faces_vec[i] += x;
            faces_vec[i + 1] += y;
            faces_vec[i + 2] += z;
        }

        return faces_vec;
    }

    std::vector<float> faces_at(glm::vec3 pos, Sides side)
    {
        return faces_at(pos.x, pos.y, pos.z, side);
    }

    void normals_on(Sides sides, float destination[3 * 6])
    {
        int i = 0;

        if (sides & Side::FRONT)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_front_normal.x;
                destination[i++] = face_front_normal.y;
                destination[i++] = face_front_normal.z;
            }
        }

        if (sides & Side::BACK)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_back_normal.x;
                destination[i++] = face_back_normal.y;
                destination[i++] = face_back_normal.z;
            }
        }

        if (sides & Side::LEFT)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_left_normal.x;
                destination[i++] = face_left_normal.y;
                destination[i++] = face_left_normal.z;
            }
        }

        if (sides & Side::RIGHT)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_right_normal.x;
                destination[i++] = face_right_normal.y;
                destination[i++] = face_right_normal.z;
            }
        }

        if (sides & Side::TOP)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_top_normal.x;
                destination[i++] = face_top_normal.y;
                destination[i++] = face_top_normal.z;
            }
        }

        if (sides & Side::BOTTOM)
        {
            for (int vertex = 0; vertex < 6; vertex++)
            {
                destination[i++] = face_bottom_normal.x;
                destination[i++] = face_bottom_normal.y;
                destination[i++] = face_bottom_normal.z;
            }
        }
        // log_debug("i = %d", i);
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