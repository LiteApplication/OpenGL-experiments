#include "testgl/worldgen.hpp"
#include "testgl/world.hpp"

#include "FastNoise.hpp"

// Avoid calling a multiple times
#define clamp(x, a, b) \
    ({typeof(x) _x = (x); \
     typeof(a) _a = (a); \
     typeof(b) _b = (b); \
     _x < _a\
                             ? _a\
                         : _x > _b ? _b\
                                   : _x; })

int roundDown(float x)
{
    return (int)x - (x < 0 && x != (int)x);
}
namespace WorldGenerator
{
    void singleBlock(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE], Voxel *simpleChunkVoxel, bool *isSimpleChunk)
    {
        *isSimpleChunk = false;
        voxels[0][0][0] = Voxel::Dirt;
    }

    void flat(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE], Voxel *simpleChunkVoxel, bool *isSimpleChunk)
    {
        *isSimpleChunk = false;
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                voxels[x][0][z] = Voxel::Grass;
            }
        }
    }

    void perlin(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE], Voxel *simpleChunkVoxel, bool *isSimpleChunk)
    {
        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetFrequency(0.01);
        noise.SetSeed(12345);

        int chunkY = ChunkPosTools::getY(pos) * CHUNK_SIZE;
        int height[CHUNK_SIZE][CHUNK_SIZE];
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                height[x][z] = clamp(roundDown(noise.GetNoise(
                                                   (float)(x + CHUNK_SIZE * ChunkPosTools::getX(pos)),
                                                   (float)(z + CHUNK_SIZE * ChunkPosTools::getZ(pos))) *
                                               50) -
                                         chunkY,
                                     0, CHUNK_SIZE);
            }
        }

        int heightTest = height[0][0];
        if (heightTest == 0 || heightTest == CHUNK_SIZE)
        {
            *isSimpleChunk = true;
            *simpleChunkVoxel = heightTest == 0 ? Voxel::Air : Voxel::Dirt;
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (height[x][z] != heightTest)
                    {
                        *isSimpleChunk = false;
                        break;
                    }
                }
                if (!*isSimpleChunk)
                    break;
            }
            if (*isSimpleChunk)
            {
                return;
            }
        }
        *isSimpleChunk = false;

        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                for (int y = 0; y < height[x][z]; y++)
                {
                    voxels[x][y][z] = Voxel::Dirt;
                }
            }
        }
    }
    void full(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE], Voxel *simpleChunkVoxel, bool *isSimpleChunk)
    {
        *isSimpleChunk = true;
        *simpleChunkVoxel = Voxel::Dirt;
    }

    void classic(ChunkPos pos, Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE], Voxel *simpleChunkVoxel, bool *isSimpleChunk)
    {
        FastNoiseLite n;
        n.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        n.SetSeed(27012004);
        n.SetFrequency(0.02);
        n.SetFractalType(FastNoiseLite::FractalType_FBm);
        n.SetFractalOctaves(2);
        n.SetFractalLacunarity(2.0);
        n.SetFractalGain(0.5);
        n.SetFractalWeightedStrength(7.0);

        const int amplitude = 50;

        int chunkY = ChunkPosTools::getY(pos) * CHUNK_SIZE;
        int height[CHUNK_SIZE][CHUNK_SIZE];
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                height[x][z] = clamp(roundDown(n.GetNoise(
                                                   (float)(x + CHUNK_SIZE * ChunkPosTools::getX(pos)),
                                                   (float)(z + CHUNK_SIZE * ChunkPosTools::getZ(pos))) *
                                                   amplitude -
                                               amplitude / 10) -
                                         chunkY,
                                     0, CHUNK_SIZE);
            }
        }

        int heightTest = height[0][0];
        // We want to keep 1 block of grass on top of the dirt so no -1 here
        if (heightTest == 0 || heightTest == CHUNK_SIZE)
        {
            *isSimpleChunk = true;
            *simpleChunkVoxel = heightTest == 0 ? Voxel::Air : Voxel::Dirt;
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (height[x][z] != heightTest)
                    {
                        *isSimpleChunk = false;
                        break;
                    }
                }
                if (!*isSimpleChunk)
                    break;
            }
            if (*isSimpleChunk)
            {
                return;
            }
        }
        *isSimpleChunk = false;

        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                for (int y = 0; y < height[x][z] - 1; y++)
                {
                    voxels[x][y][z] = Voxel::Dirt;
                }
                if (height[x][z] > 0)
                {
                    voxels[x][height[x][z] - 1][z] = Voxel::Grass;
                }

                if (ChunkPosTools::getY(pos) < 0)
                {
                    if (height[x][z] > 0)
                    {
                        voxels[x][height[x][z] - 1][z] = Voxel::Sand;
                    }
                    if (height[x][z] < CHUNK_SIZE)
                    {

                        for (int y = height[x][z]; y < CHUNK_SIZE - 2; y++)
                        {
                            voxels[x][y][z] = Voxel::Water;
                        }
                    }
                }
            }
        }
    }

} // namespace WorldGenerator