#pragma once

#include <map>
#include <tuple>
#include <mutex>
#include <queue>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "testgl/chunk.hpp"
#include "testgl/constants.hpp"
#include "testgl/worldgen.hpp"

class Chunk;

class World
{
private:
    // Store all the loaded chunks
    std::map<ChunkPos, Chunk *> chunks;

    std::mutex chunksMutex;

    // Pointer to the player position vector
    glm::vec3 *playerPos;

    // Chunk the player is in as of the last tick
    ChunkPos playerChunk;

    // World generation function
    WorldGenerator::function_t worldGenerator;

    // Fill the priority queue with chunks to load
    void nextChunkToLoad();

    // Get the chunk at a certain position
    Chunk *getChunk(ChunkPos pos);

    // Load a chunk at a certain position
    // This will run worldGenerator for each block in it
    // No mesh or block occlusion will be calculated
    // The chunk has to be within the render distance of the player
    void createChunk(ChunkPos pos);

    // Loading priority stuff:
    typedef std::pair<ChunkPos, int> ChunkPosWithDist; // ChunkPos with distance from player
    typedef std::pair<Chunk *, int> ChunkWithDist;     // Chunk with distance from player
    // Comparison function for the priority queues
    struct ChunkWithDistCompare
    {
        bool operator()(const ChunkWithDist &a, const ChunkWithDist &b)
        {
            return a.second < b.second;
        }
    };

    // Comparison function for the priority queues
    struct ChunkPosWithDistCompare
    {
        bool operator()(const ChunkPosWithDist &a, const ChunkPosWithDist &b)
        {
            return a.second < b.second;
        }
    };
    // The queue of chunks to be loaded
    std::priority_queue<ChunkPosWithDist, std::vector<ChunkPosWithDist>, ChunkPosWithDistCompare> chunksToLoad;
    // The queue of chunks to be face occluded
    std::priority_queue<ChunkWithDist, std::vector<ChunkWithDist>, ChunkWithDistCompare> chunksToFaceOcclude;
    // The queue of chunks to be meshed
    std::priority_queue<ChunkWithDist, std::vector<ChunkWithDist>, ChunkWithDistCompare> chunksToMesh;
    // The queue of chunks to be uploaded
    std::mutex chunksToUploadMutex;
    std::priority_queue<ChunkWithDist, std::vector<ChunkWithDist>, ChunkWithDistCompare> chunksToUpload;

    // Fast distance² calculation
    int distanceFunction(ChunkPos pos);
    int distanceFunction(Chunk *chunk);

    // Make elements of the priority queues
    ChunkPosWithDist makeChunkPosWithDist(ChunkPos pos);
    ChunkWithDist makeChunkWithDist(Chunk *chunk);

public:
    // World constructor
    World(glm::vec3 *playerPos, WorldGenerator::function_t worldGenerator);

    // "Now I am become Death, the Destroyer of Worlds” - Sanskrit
    ~World();

    // Set a voxel value anywhere inside the loaded chunks
    // Returns false if the position is outside loaded chunks
    bool setVoxel(int x, int y, int z, Voxel value);

    // Get a voxel value anywhere inside the loaded chunks
    // Returns VOXEL_INVALID if the position is outside loaded chunks
    Voxel getVoxel(int x, int y, int z);

    // Check if a chunk is loaded
    bool isChunkLoaded(ChunkPos pos);

    // Draw all the chunks on the screen
    // Must be ran from the main thread
    void draw(Shader *shader);

    // Load the `numberOfChunks` due chunks closest to the player
    void loadChunks(int numberOfChunks);

    // Update the side occlusion map for the `numberOfChunks` due chunks closest to the player
    void updateSideOcclusion(int numberOfChunks);

    // Update the mesh for the `numberOfChunks` due chunks closest to the player
    void updateMesh(int numberOfChunks);

    // Upload the mesh of the `numberOfChunks` due chunks closest to the player
    // Must be ran from the main thread
    void uploadMesh(int numberOfChunks);

    // Loads all the chunks at once, for the first time
    // Must be ran from the main thread
    void loadAllChunks();

    // Discard buffers for chunks that are too far away and schedule them for deletion
    // Must be ran from the main thread
    void discardChunks();

    // Delete the chunks from the world data structure
    void deleteChunks();

    // Load chunks, calculate side occlusion, generate meshes
    // This is the main loop, ran on a separate thread
    void tick();

    // Upload meshes to GPU, draw chunks
    // This is the graphical loop, ran on the main thread
    void graphicalTick(Shader *shader);

    // Number of ticks since the world was created
    int nTicks = 0;

    // Number of loaded chunks
    int getLoadedChunks() { return chunks.size(); }

    // Put functions for the priority queues here
    // They are public so they can be used by the chunks themselves
    void addToLoadQueue(ChunkPos pos);
    void addToFaceOcclusionQueue(Chunk *chunk);
    void addToMeshQueue(Chunk *chunk);
    void addToUploadQueue(Chunk *chunk);
};
