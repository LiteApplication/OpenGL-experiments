#include "testgl/world.hpp"

#define VIEW_DISTANCE_2 VIEW_DISTANCE *VIEW_DISTANCE

namespace ChunkPosTools
{

    int squaredDist(ChunkPos pos1, ChunkPos pos2)
    {
        ChunkPos delta = pos1 - pos2;
        int x2 = getX(delta);
        int y2 = getY(delta);
        int z2 = getZ(delta);
        return x2 * x2 + y2 * y2 + z2 * z2;
    }

    ChunkPos fromWorldPos(int x, int y, int z)
    {
        // We cannot just divide by CHUNK_SIZE because it would round towards 0
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

    int getX(ChunkPos pos)
    {
        return std::get<0>(pos);
    }

    int getY(ChunkPos pos)
    {
        return std::get<1>(pos);
    }

    int getZ(ChunkPos pos)
    {
        return std::get<2>(pos);
    }
} // namespace ChunkPosTools

using namespace ChunkPosTools;

ChunkPos World::nextChunkToLoad()
{
    // Manhatten distance
    for (int dist = 1; dist <= VIEW_DISTANCE; dist++)
    {
        for (int i = -dist; i <= dist; i++)
        {
            for (int j = -dist; j <= dist; j++)
            {
                for (int k = -dist; k <= dist; k++)
                {
                    if (abs(i) + abs(j) * HEIGHT_VIEW_REDUCTION + abs(k) != dist)
                        continue;
                    ChunkPos pos = playerChunk + ChunkPos(i, j, k);
                    if (isChunkLoaded(pos))
                        continue;
                    return pos;
                }
            }
        }
    }
    return playerChunk;
}

bool World::isChunkLoaded(ChunkPos pos)
{
    return chunks.find(pos) != chunks.end();
}

World::World(glm::vec3 *playerPos, WorldGenerator::function_t worldGenerator) : playerPos(playerPos), chunks(), worldGenerator(worldGenerator)
{
    playerChunk = fromWorldPos(*playerPos);
}

Chunk *World::getChunk(ChunkPos pos)
{
    if (chunks.find(pos) == chunks.end())
        return nullptr;
    return &chunks[pos];
}

void World::createChunk(ChunkPos pos)
{
    if (chunks.find(pos) != chunks.end())
    {
        // TODO : WARNING
        // log_warn("Tried to create chunk that already exists (%d, %d, %d)", getX(pos), getY(pos), getZ(pos));
        return;
    }
    log_debug("Creating chunk (%d, %d, %d)", getX(pos), getY(pos), getZ(pos));
    chunks[pos] = Chunk(getX(pos), getY(pos), getZ(pos));
    ChunkPos relativePos = pos - playerChunk;
    chunks[pos].populate(worldGenerator);
}

void World::deleteChunk(ChunkPos pos)
{
    chunks.erase(pos);
}

World::~World()
{
    chunks.clear();
}

void World::loadChunks(int numberOfChunks)
{
    // Always load the chunk the player is in
    if (!isChunkLoaded(playerChunk))
    {
        createChunk(playerChunk);
    }
    for (int i = 0; i < numberOfChunks; i++)
    {
        ChunkPos pos = nextChunkToLoad();
        if (pos == playerChunk)
            return; // We're done
        createChunk(pos);
    }
}

void World::updateSideOcclusion(int numberOfChunks)
{
    // Iterate through `chunks` and update the side occlusion of each chunk
    for (auto &[pos, chunk] : chunks)
    {
        if (chunk.getNeedsSideOcclusionUpdate())
        {
            chunk.calculateNeedsDraw();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
}

void World::updateMesh(int numberOfChunks)
{
    // Iterate through `chunks` and update the mesh of each chunk
    for (auto &[pos, chunk] : chunks)
    {
        if (chunk.getNeedsMeshUpdate())
        {
            chunk.generateMesh();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
}

void World::uploadMesh(int numberOfChunks)
{
    // Iterate through `chunks` and upload the mesh of each chunk
    for (auto &[pos, chunk] : chunks)
    {
        if (chunk.getNeedsMeshUpload())
        {
            chunk.uploadMesh();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
}

void World::discardChunks()
{
    // Iterate through `chunks` and delete the chunks that are too far away from the player
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        ChunkPos pos = it->first;
        if (squaredDist(pos, playerChunk) > VIEW_DISTANCE_2)
        {
            log_debug("Discarding chunk (%d, %d, %d)", getX(pos), getY(pos), getZ(pos));
            // Check if the chunk is represented in the loaded chunks map
            it = chunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void World::draw(Shader *shader)
{
    // Iterate through `chunks` and draw each chunk
    for (auto &[pos, chunk] : chunks)
    {
        chunk.draw(shader);
    }
}

bool World::setVoxel(int x, int y, int z, Voxel value)
{
    ChunkPos chunkPos = fromWorldPos(x, y, z);
    Chunk *chunk = getChunk(chunkPos);
    if (chunk == nullptr)
        return false;
    chunk->setVoxel(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, value);
    return true;
}

Voxel World::getVoxel(int x, int y, int z)
{
    ChunkPos chunkPos = fromWorldPos(x, y, z);
    Chunk *chunk = getChunk(chunkPos);
    if (chunk == nullptr)
        return VOXEL_INVALID;
    return chunk->getVoxel(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
}

void World::tick()
{
    // Update the player position and chunk
    playerChunk = fromWorldPos(*playerPos);

    // log_debug("Player chunk : (%d, %d, %d)", getX(playerChunk), getY(playerChunk), getZ(playerChunk));

    // Load the chunks around the player
    loadChunks(CHUNK_GEN_PER_TICK);

    // Update the side occlusion of the chunks around the player
    updateSideOcclusion(CHUNK_MESH_PER_TICK);

    // Update the mesh of the chunks around the player
    updateMesh(CHUNK_MESH_PER_TICK);
}

void World::graphicalTick(Shader *shader)
{
    // Discard chunks that are too far away from the player
    discardChunks();

    // Upload the mesh of the chunks around the player
    uploadMesh(CHUNK_GPU_UPLOAD_PER_FRAME);

    // Draw the chunks
    draw(shader);
}