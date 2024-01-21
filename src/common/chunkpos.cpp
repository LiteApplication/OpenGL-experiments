#include "testgl/chunkpos.hpp"
#include "testgl/constants.hpp"

namespace ChunkPosTools
{
    bool chunkTooFar(ChunkPos pos, ChunkPos playerPos)
    {
        return abs(getX(pos) - getX(playerPos)) > VIEW_DISTANCE || abs(getY(pos) - getY(playerPos)) > VIEW_DISTANCE || abs(getZ(pos) - getZ(playerPos)) > VIEW_DISTANCE;
    }

    ChunkPos fromWorldPos(int x, int y, int z)
    {
        // We cannot just divide by CHUNK_SIZE because it would round towards 0 even for negative numbers
        // We need to round down
        int x2 = x < 0 ? (x - CHUNK_SIZE + 1) / CHUNK_SIZE : x / CHUNK_SIZE;
        int y2 = y < 0 ? (y - CHUNK_SIZE + 1) / CHUNK_SIZE : y / CHUNK_SIZE;
        int z2 = z < 0 ? (z - CHUNK_SIZE + 1) / CHUNK_SIZE : z / CHUNK_SIZE;
        return ChunkPos(x2, y2, z2);
    }

    ChunkPos fromWorldPos(glm::vec3 pos)
    {
        return fromWorldPos(pos.x, pos.y, pos.z);
    }

    glm::vec3 toWorldPos(ChunkPos pos)
    {
        return glm::vec3(getX(pos) * CHUNK_SIZE, getY(pos) * CHUNK_SIZE, getZ(pos) * CHUNK_SIZE);
    }

    bool operator==(ChunkPos a, ChunkPos b)
    {
        return getX(a) == getX(b) && getY(a) == getY(b) && getZ(a) == getZ(b);
    }

    bool operator!=(ChunkPos a, ChunkPos b)
    {
        return !(a == b);
    }

    ChunkPos operator+(ChunkPos a, ChunkPos b)
    {
        return ChunkPos(getX(a) + getX(b), getY(a) + getY(b), getZ(a) + getZ(b));
    }

    ChunkPos operator-(ChunkPos a, ChunkPos b)
    {
        return ChunkPos(getX(a) - getX(b), getY(a) - getY(b), getZ(a) - getZ(b));
    }
} // namespace ChunkPosTools
