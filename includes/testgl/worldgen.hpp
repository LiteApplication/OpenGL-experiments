#pragma once

#include <functional>
#include "testgl/constants.hpp"
#include "testgl/voxel.hpp"

typedef std::tuple<int, int, int> ChunkPos;
typedef Voxel ChunkData[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

namespace WorldGenerator
{
    typedef std::function<void(ChunkPos, ChunkData)> function_t;

    void singleBlock(ChunkPos pos, ChunkData voxels);
    void flat(ChunkPos pos, ChunkData voxels);
} // namespace WorldGenerator