#pragma once

#include <functional>
#include "testgl/constants.hpp"
#include "testgl/voxel.hpp"
#include "testgl/chunkpos.hpp"

typedef Voxel ChunkData[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

namespace WorldGenerator
{
    typedef std::function<void(ChunkPos, ChunkData, Voxel *, bool *)> function_t;

    void singleBlock(ChunkPos pos, ChunkData voxels, Voxel *simpleChunkVoxel, bool *isSimpleChunk);
    void flat(ChunkPos pos, ChunkData voxels, Voxel *simpleChunkVoxel, bool *isSimpleChunk);
    void perlin(ChunkPos pos, ChunkData voxels, Voxel *simpleChunkVoxel, bool *isSimpleChunk);

    void full(ChunkPos pos, ChunkData voxels, Voxel *simpleChunkVoxel, bool *isSimpleChunk);
    void classic(ChunkPos pos, ChunkData voxels, Voxel *simpleChunkVoxel, bool *isSimpleChunk);
} // namespace WorldGenerator