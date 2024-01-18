#include "testgl/worldgen.hpp"
#include "testgl/world.hpp"

#include "FastNoise.hpp"

namespace WorldGenerator
{
    void singleBlock(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        voxels[0][0][0] = VOXEL_BROWN;
    }

    void flat(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        if (ChunkPosTools::getY(pos) == 0)
        {

            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    voxels[x][0][z] = (x + z) % 4 + 1;
                }
            }
        }
    }

    void perlin(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetFrequency(0.01);
        noise.SetSeed(12345);
        int chunkY = ChunkPosTools::getY(pos) * CHUNK_SIZE;
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                int height = (int)(noise.GetNoise((float)(x + CHUNK_SIZE * ChunkPosTools::getX(pos)), (float)(z + CHUNK_SIZE * ChunkPosTools::getZ(pos))) * 10);
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    if (y + chunkY < height)
                        voxels[x][y][z] = VOXEL_BROWN;
                    else
                        voxels[x][y][z] = VOXEL_NONE;
                }
            }
        }
    }

    void full(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE])
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    voxels[x][y][z] = VOXEL_BROWN;
                }
            }
        }
    }

} // namespace WorldGenerator