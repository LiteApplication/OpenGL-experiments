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

    bool chunkTooFar(ChunkPos pos, ChunkPos playerPos)
    {
        return abs(getX(pos) - getX(playerPos)) > VIEW_DISTANCE || abs(getY(pos) - getY(playerPos)) > VIEW_DISTANCE || abs(getZ(pos) - getZ(playerPos)) > VIEW_DISTANCE;
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
    // Load a square of chunks around the player
    // The square is centered on the player
    // The square is of size 2 * VIEW_DISTANCE + 1

    // Load closest chunks first
    for (int dist = 0; dist <= VIEW_DISTANCE; dist++)
    {
        // Load the chunks on the edges
        for (int x = -dist; x <= dist; x++)
        {
            for (int y = -dist / HEIGHT_VIEW_REDUCTION; y <= dist / HEIGHT_VIEW_REDUCTION; y++)
            {
                for (int z = -dist; z <= dist; z++)
                {
                    // Skip the corners
                    if (abs(x) == dist && abs(y) == dist / HEIGHT_VIEW_REDUCTION && abs(z) == dist)
                        continue;
                    ChunkPos pos = playerChunk + ChunkPos(x, y, z);
                    if (!isChunkLoaded(pos))
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
    return chunks[pos];
}

void World::createChunk(ChunkPos pos)
{
    if (chunks.find(pos) != chunks.end())
    {
        log_warn("Tried to create chunk that already exists (%d, %d, %d)", getX(pos), getY(pos), getZ(pos));
        return;
    }
    // log_debug("Creating chunk (%d, %d, %d)", getX(pos), getY(pos), getZ(pos));
    Chunk *chunk = new Chunk(getX(pos), getY(pos), getZ(pos));
    chunksMutex.lock();
    chunks[pos] = chunk;
    chunksMutex.unlock();
    ChunkPos relativePos = pos - playerChunk;
    chunk->populate(worldGenerator);

    // Update the chunk's side occlusion with the neighboring chunks
    for (int side = 0; side < 6; side++)
    {
        Side oppositeSide = (Side)(1 << side);
        Side sideEnum = opposite_side(oppositeSide);
        // Get the position of the neighbor chunk
        ChunkPos neighborPos = pos + dirFromSide(sideEnum);
        // Check if the neighbor chunk is loaded
        Chunk *neighbor = getChunk(neighborPos);
        if (neighbor == nullptr)
            continue;
        chunk->getObstructions(sideEnum, neighbor->obstructions[side]);
        neighbor->getObstructions(oppositeSide, chunk->obstructions[side_to_index(sideEnum)]);
        // Update the neighbor chunk
        neighbor->setNeedsSideOcclusionUpdate(true);
    }
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
        if (chunk->getNeedsSideOcclusionUpdate())
        {
            chunk->calculateNeedsDraw();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
        Sides edge = chunk->getEdgeChanged();
        if (edge == Side::NONE)
            continue;

        // Loop over the sides
        for (int side = 0; side < 6; side++)
        {
            Side oppositeSide = (Side)(1 << side);
            Side sideEnum = opposite_side(oppositeSide);
            // Check if the side has changed
            if (!(edge & sideEnum))
                continue;
            // Get the position of the neighbor chunk
            ChunkPos neighborPos = pos + dirFromSide(sideEnum);
            // Check if the neighbor chunk is loaded
            Chunk *neighbor = getChunk(neighborPos);
            if (neighbor == nullptr)
                continue;
            chunk->getObstructions(sideEnum, neighbor->obstructions[side]);
            // Update the neighbor chunk
            neighbor->setNeedsSideOcclusionUpdate(true);
        }
        chunk->setEdgeChanged(Side::NONE);
    }
}

void World::updateMesh(int numberOfChunks)
{
    // Iterate through `chunks` and update the mesh of each chunk
    for (auto &[pos, chunk] : chunks)
    {
        if (chunk->getNeedsMeshUpdate())
        {
            chunk->generateMesh();
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
        if (chunk->getNeedsMeshUpload())
        {
            chunk->uploadMesh();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
}

void World::discardChunks()
{
    // Iterate through `chunks` and delete the chunks that are too far away from the player
    for (auto &[pos, chunk] : chunks)
    {
        if (chunkTooFar(pos, playerChunk))
            chunk->discard();
    }
}

void World::deleteChunks()
{
    // Iterate through `chunks` and delete the chunks that are too far away from the player
    bool needsDeletion = false;
    for (auto &[pos, chunk] : chunks)
    {
        needsDeletion |= chunk->getScheduleForDeletion();
    }
    if (!needsDeletion)
        return;

    chunksMutex.lock();
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        if (it->second->getScheduleForDeletion())
        {
            delete it->second;
            it = chunks.erase(it);
        }
        else
            it++;
    }

    chunksMutex.unlock();
}

void World::draw(Shader *shader)
{
    // Iterate through `chunks` and draw each chunk
    for (auto &[pos, chunk] : chunks)
    {
        chunk->draw(shader);
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

void World::loadAllChunks()
{
    loadChunks(VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE * 8);

    // Update the side occlusion of the chunks around the player
    updateSideOcclusion(VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE * 8);

    // Update the mesh of the chunks around the player
    updateMesh(VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE * 8);

    // Upload the mesh of the chunks around the player
    uploadMesh(VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE * 8);
}

void World::tick()
{
    // Check if the player has moved to another chunk
    ChunkPos newPlayerChunk = fromWorldPos(*playerPos);
    if (newPlayerChunk != playerChunk)
    {
        (void)0;
    }

    // Update the player position and chunk
    playerChunk = fromWorldPos(*playerPos);

    // Delete the chunks that are too far away from the player
    deleteChunks();

    // log_debug("Player chunk : (%d, %d, %d)", getX(playerChunk), getY(playerChunk), getZ(playerChunk));

    // Load the chunks around the player
    loadChunks(CHUNK_GEN_PER_TICK);

    // Update the side occlusion of the chunks around the player
    updateSideOcclusion(CHUNK_MESH_PER_TICK);

    // Update the mesh of the chunks around the player
    updateMesh(CHUNK_MESH_PER_TICK);

    nTicks++;
}

void World::graphicalTick(Shader *shader)
{
    // Prevent the tick thread from deleting chunks while we're drawing
    chunksMutex.lock();

    // Discard chunks that are too far away from the player
    discardChunks();

    // Upload the mesh of the chunks around the player
    uploadMesh(CHUNK_GPU_UPLOAD_PER_FRAME);

    // Draw the chunks
    draw(shader);

    // Unlock the mutex
    chunksMutex.unlock();
}