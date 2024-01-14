#include "testgl/worldgen.hpp"

namespace WorldGenerator
{
    void singleBlock(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        voxels[0][0][0] = VOXEL_BROWN;
    }

    void flat(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                voxels[x][0][z] = VOXEL_BROWN;
            }
        }
    }

} // namespace WorldGenerator