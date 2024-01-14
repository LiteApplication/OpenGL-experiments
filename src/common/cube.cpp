#include "testgl/cube.hpp"

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

    std::vector<glm::vec3> normals_on(Sides sides)
    {
        std::vector<glm::vec3> normals_vec;

        if (sides & Side::FRONT)
        {
            normals_vec.insert(normals_vec.end(), face_front_normal);
        }

        if (sides & Side::BACK)
        {
            normals_vec.insert(normals_vec.end(), face_back_normal);
        }

        if (sides & Side::LEFT)
        {
            normals_vec.insert(normals_vec.end(), face_left_normal);
        }

        if (sides & Side::RIGHT)
        {
            normals_vec.insert(normals_vec.end(), face_right_normal);
        }

        if (sides & Side::TOP)
        {
            normals_vec.insert(normals_vec.end(), face_top_normal);
        }

        if (sides & Side::BOTTOM)
        {
            normals_vec.insert(normals_vec.end(), face_bottom_normal);
        }

        return normals_vec;
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