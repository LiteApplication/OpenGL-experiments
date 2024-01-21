#pragma once

#include <tuple>
#include <glm/glm.hpp>
#include <functional>

typedef std::tuple<int, int, int>
    ChunkPos;

namespace ChunkPosTools
{

    // Convert world coordinates to chunk coordinates
    ChunkPos fromWorldPos(int x, int y, int z);
    // Convert world coordinates to chunk coordinates
    ChunkPos fromWorldPos(glm::vec3 pos);
    // Convert chunk coordinates to world coordinates
    glm::vec3 toWorldPos(ChunkPos pos);

    // Check if a chunk is too far from the player
    bool chunkTooFar(ChunkPos pos, ChunkPos playerPos);

    // Getters
    constexpr int getX(ChunkPos pos) { return std::get<0>(pos); }
    constexpr int getY(ChunkPos pos) { return std::get<1>(pos); }
    constexpr int getZ(ChunkPos pos) { return std::get<2>(pos); }

    // Operators
    bool operator==(ChunkPos, ChunkPos);
    bool operator!=(ChunkPos, ChunkPos);
    ChunkPos operator+(ChunkPos, ChunkPos);
    ChunkPos operator-(ChunkPos, ChunkPos);

    // Hash function for ChunkPos
    struct ChunkPosHash
    {
        std::size_t operator()(const ChunkPos &k) const
        {
            return std::hash<int>()(getX(k)) ^ std::hash<int>()(getY(k)) ^ std::hash<int>()(getZ(k));
        }
    };
}
