#include "testgl/world.hpp"

#define VIEW_DISTANCE_2 VIEW_DISTANCE *VIEW_DISTANCE

using namespace ChunkPosTools;

void World::nextChunkToLoad()
{
    // Load a square of chunks around the player
    // The square is centered on the player
    // The square is of size 2 * VIEW_DISTANCE + 1

    // Clear the load queue
    std::priority_queue<ChunkPosWithDist, std::vector<ChunkPosWithDist>, ChunkPosWithDistCompare> empty;
    std::swap(chunksToLoad, empty);

    for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
    {
        for (int y = -VIEW_DISTANCE / HEIGHT_VIEW_REDUCTION; y <= VIEW_DISTANCE / HEIGHT_VIEW_REDUCTION; y++)
        {
            for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
            {
                // Skip the corners
                if (abs(x) == VIEW_DISTANCE && abs(y) == VIEW_DISTANCE / HEIGHT_VIEW_REDUCTION && abs(z) == VIEW_DISTANCE)
                    continue;
                ChunkPos pos = playerChunk + ChunkPos(x, y, z);
                if (!isChunkLoaded(pos))
                    addToLoadQueue(pos);
            }
        }
    }
}

bool World::isChunkLoaded(ChunkPos pos)
{
    return chunks.find(pos) != chunks.end();
}

World::World(glm::vec3 *playerPos, WorldGenerator::function_t worldGenerator) : playerPos(playerPos), chunks(), worldGenerator(worldGenerator), nTicks(0)
{
    playerChunk = fromWorldPos(*playerPos);
    chunksToLoad.push(makeChunkPosWithDist(playerChunk));
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
    Chunk *chunk = new Chunk(getX(pos), getY(pos), getZ(pos), this);
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
        addToFaceOcclusionQueue(neighbor);
    }
    chunk->setEdgeChanged(Side::NONE); // We just updated the neighbors, so no edge has changed
}

World::~World()
{
    for (auto &[pos, chunk] : chunks)
    {
        chunk->discard(); // Delete the buffers
        delete chunk;
    }
    chunks.clear();
}

int World::distanceFunction(ChunkPos pos)
{
    ChunkPos relativePos = pos - playerChunk;
    // Negative distance so that the chunks are sorted from closest to farthest
    return -(getX(relativePos) * getX(relativePos) + getY(relativePos) * getY(relativePos) + getZ(relativePos) * getZ(relativePos));
}

int World::distanceFunction(Chunk *chunk)
{
    return distanceFunction(chunk->getPos());
}

World::ChunkPosWithDist World::makeChunkPosWithDist(ChunkPos pos)
{
    return ChunkPosWithDist(pos, distanceFunction(pos));
}

World::ChunkWithDist World::makeChunkWithDist(Chunk *chunk)
{
    return ChunkWithDist(chunk, distanceFunction(chunk));
}
void World::addToLoadQueue(ChunkPos pos)
{
    chunksToLoad.push(makeChunkPosWithDist(pos));
}

void World::addToFaceOcclusionQueue(Chunk *chunk)
{
    chunksToFaceOcclude.push(makeChunkWithDist(chunk));
}

void World::addToMeshQueue(Chunk *chunk)
{
    chunksToMesh.push(makeChunkWithDist(chunk));
}

void World::addToUploadQueue(Chunk *chunk)
{
    // This will be popped by the main thread so we need to protect it
    chunksToUploadMutex.lock();
    chunksToUpload.push(makeChunkWithDist(chunk));
    chunksToUploadMutex.unlock();
}

void World::loadChunks(int numberOfChunks)
{
    // Always load the chunk the player is in
    if (!isChunkLoaded(playerChunk))
    {
        createChunk(playerChunk);
        numberOfChunks--;
    }
    nextChunkToLoad();
    // Iterate through `chunks` and load each chunk
    while (!chunksToLoad.empty())
    {
        ChunkPos pos = chunksToLoad.top().first;
        chunksToLoad.pop();

        if (!isChunkLoaded(pos))
        {
            createChunk(pos);
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
}

void World::updateSideOcclusion(int numberOfChunks)
{
    int counter = numberOfChunks;
    // First we update the side occlusion of the needed chunks so that we can
    // update the side occlusion of the neighboring chunks later
    for (auto &[pos, chunk] : chunks)
    {
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
            addToFaceOcclusionQueue(neighbor);
        }
        chunk->setEdgeChanged(Side::NONE);

        counter--;
        if (counter == 0)
            break;
    }

    counter = numberOfChunks;
    // Iterate through `chunks` and update the side occlusion of each chunk
    while (!chunksToFaceOcclude.empty())
    {
        Chunk *chunk = chunksToFaceOcclude.top().first;
        chunksToFaceOcclude.pop();
        if (chunk == nullptr)
            continue;
        if (chunk->getNeedsSideOcclusionUpdate())
        {
            chunk->calculateNeedsDraw();
            counter--;
            if (counter == 0)
                break;
        }
    }
}

void World::updateMesh(int numberOfChunks)
{
    // Iterate through `chunks` and update the mesh of each chunk
    while (!chunksToMesh.empty())
    {
        Chunk *chunk = chunksToMesh.top().first;
        chunksToMesh.pop();
        if (chunk == nullptr)
            continue;
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
    // TODO: Use a mutex to protect the queue
    chunksToUploadMutex.lock();
    while (!chunksToUpload.empty())
    {
        Chunk *chunk = chunksToUpload.top().first;
        chunksToUpload.pop();
        if (chunk == nullptr)
            continue;
        if (chunk->getNeedsMeshUpload())
        {
            chunk->uploadMesh();
            numberOfChunks--;
            if (numberOfChunks == 0)
                break;
        }
    }
    chunksToUploadMutex.unlock();
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
// https://stackoverflow.com/a/29325258/15860367
// Provides access to the container of the priority_queue
template <class ADAPTER>
typename ADAPTER::container_type &get_container(ADAPTER &a)
{
    struct hack : ADAPTER
    {
        static typename ADAPTER::container_type &get(ADAPTER &a)
        {
            return a.*&hack::c;
        }
    };
    return hack::get(a);
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

    // Iterate through all the queued chunks and replace them with nullptr
    // This is done to prevent the tick thread to update a chunk that is no longer in the world
    std::vector<ChunkWithDist> &chunksToFaceOccludeContainer = get_container(chunksToFaceOcclude);
    for (auto &chunk : chunksToFaceOccludeContainer)
    {
        if (chunk.first == nullptr)
            continue;
        if (chunk.first->getScheduleForDeletion())
            chunk.first = nullptr;
    }
    std::vector<ChunkWithDist> &chunksToMeshContainer = get_container(chunksToMesh);
    for (auto &chunk : chunksToMeshContainer)
    {
        if (chunk.first == nullptr)
            continue;
        if (chunk.first->getScheduleForDeletion())
            chunk.first = nullptr;
    }

    chunksToUploadMutex.lock();
    std::vector<ChunkWithDist> &chunksToUploadContainer = get_container(chunksToUpload);
    for (auto &chunk : chunksToUploadContainer)
    {
        if (chunk.first == nullptr)
            continue;
        if (chunk.first->getScheduleForDeletion())
            chunk.first = nullptr;
    }
    chunksToUploadMutex.unlock();

    // Delete the chunks that are scheduled for deletion
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
        if (!isChunkLoaded(newPlayerChunk))
        {
            createChunk(newPlayerChunk);
            log_warn("Player moved to unloaded chunk (%d, %d, %d)", getX(newPlayerChunk), getY(newPlayerChunk), getZ(newPlayerChunk));
        }

        // Get the chunk and print its infos
        Chunk *chunk = getChunk(newPlayerChunk);
        if (chunk == nullptr)
        {
            log_warn("Player moved to unloaded chunk (%d, %d, %d)", getX(newPlayerChunk), getY(newPlayerChunk), getZ(newPlayerChunk));
        }
        else
        {
            chunk->print_info();
        }
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
