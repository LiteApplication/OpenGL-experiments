#include "testgl/chunk.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "testgl/constants.hpp"
#include "testgl/voxel.hpp"
#include "testgl/cube.hpp"

Chunk::Chunk(int x, int y, int z) : m_x(x), m_y(y), m_z(z), meshSize(0), VAO(0), VBO(0), EBO(0)
{
    m_x = x;
    m_y = y;
    m_z = z;

    hasBuffer = false;
    needsSideOcclusionUpdate = false;
    needsMeshUpdate = false;
    needsMeshUpload = false;
    scheduledForDeletion = false;

    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(x, y, z) * (float)CHUNK_SIZE);
    m_invModelMatrix = glm::inverse(m_modelMatrix);

    meshSize = 0;

    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            for (int k = 0; k < CHUNK_SIZE; k++)
            {
                voxels[i][j][k] = VOXEL_NONE;
                needsDraw[i][j][k] = Side::NONE;
            }
        }
    }

    isSimpleChunk = true;
    simpleChunkVoxel = VOXEL_NONE;

    edgeChanged = Side::NONE;

    for (int side = 0; side < 6; side++)
    {
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                obstructions[side][i][j] = true;
            }
        }
    }
}

Chunk::~Chunk()
{
    meshVertices.clear();
    meshNormals.clear();
    meshColors.clear();

    // log_debug("Discarding chunk (%d, %d, %d)", m_x, m_y, m_z);
}

Voxel Chunk::getVoxel(int x, int y, int z)
{
    if (isSimpleChunk)
        return simpleChunkVoxel;
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        log_warn("getVoxel : out of bounds (%d, %d, %d)", x, y, z);
        return VOXEL_INVALID;
    }
    return voxels[x][y][z];
}

void Chunk::setVoxel(int x, int y, int z, Voxel value)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        log_warn("setVoxel : out of bounds (%d, %d, %d)", x, y, z);
        return;
    }

    if (isSimpleChunk)
    {
        if (value == simpleChunkVoxel)
            return;
        else
        {
            isSimpleChunk = false;
            for (int i = 0; i < CHUNK_SIZE; i++)
            {
                for (int j = 0; j < CHUNK_SIZE; j++)
                {
                    for (int k = 0; k < CHUNK_SIZE; k++)
                    {
                        voxels[i][j][k] = simpleChunkVoxel;
                    }
                }
            }
            needsMeshUpdate = true;
        }
    }
    // Check if the voxel is actually changing
    if (voxels[x][y][z] == value)
        return;

    // Check if the voxel is on the edge of the chunk if the mesh shape changes
    if (value == VOXEL_NONE != voxels[x][y][z] == VOXEL_NONE) // XOR
    {
        if (x == 0)
            edgeChanged |= Side::LEFT;
        else if (x == CHUNK_SIZE - 1)
            edgeChanged |= Side::RIGHT;

        if (y == 0)
            edgeChanged |= Side::BOTTOM;
        else if (y == CHUNK_SIZE - 1)
            edgeChanged |= Side::TOP;

        if (z == 0)
            edgeChanged |= Side::BACK;
        else if (z == CHUNK_SIZE - 1)
            edgeChanged |= Side::FRONT;

        needsSideOcclusionUpdate = true;
    }
    else
        needsMeshUpdate = true; // Skip side occlusion update

    voxels[x][y][z] = value;
}

void Chunk::populate(WorldGenerator::function_t worldGenerator)
{
    worldGenerator(getPos(), voxels, &simpleChunkVoxel, &isSimpleChunk);
    needsSideOcclusionUpdate = true; // we now need to calculate which sides are worth drawing
}

void Chunk::setVoxelLayer(int y, Voxel value)
{
    if (y < 0 || y >= CHUNK_SIZE)
    {
        log_warn("setVoxelLayer : out of bounds (%d)", y);
        return;
    }

    if (isSimpleChunk)
    {
        if (value == simpleChunkVoxel)
            return;
        else
        {
            setVoxel(0, y, 0, value); // Convert to a normal chunk
        }
    }

    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            voxels[i][y][j] = value;
        }
    }
    needsSideOcclusionUpdate = true;
    edgeChanged |= Side::LEFT | Side::RIGHT | Side::FRONT | Side::BACK;
    if (y == 0)
        edgeChanged |= Side::BOTTOM;
    else if (y == CHUNK_SIZE - 1)
        edgeChanged |= Side::TOP;
}

void Chunk::calculateNeedsDraw()
{
    needsSideOcclusionUpdate = false;
    needsMeshUpdate = false;

    int nextBlockCheck = 1; // Iterate over the voxels every block

    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            for (int k = 0; k < CHUNK_SIZE; k++)
            {
                needsDraw[i][j][k] = Side::NONE;
            }
        }
    }

    if (isSimpleChunk)
    {
        if (simpleChunkVoxel == VOXEL_NONE)
        {
            needsMeshUpdate = true;
            return;
        }
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (!(x == 0 || y == 0 || z == 0 || x == CHUNK_SIZE - 1 || y == CHUNK_SIZE - 1 || z == CHUNK_SIZE - 1))
                        continue; // Don't check the inside of the chunk

                    if (z == CHUNK_SIZE - 1 && !obstructions[0][x][y])
                        needsDraw[x][y][z] |= Side::FRONT;
                    if (z == 0 && !obstructions[1][x][y])
                        needsDraw[x][y][z] |= Side::BACK;
                    if (x == 0 && !obstructions[2][y][z])
                        needsDraw[x][y][z] |= Side::LEFT;
                    if (x == CHUNK_SIZE - 1 && !obstructions[3][y][z])
                        needsDraw[x][y][z] |= Side::RIGHT;
                    if (y == CHUNK_SIZE - 1 && !obstructions[4][x][z])
                        needsDraw[x][y][z] |= Side::TOP;
                    if (y == 0 && !obstructions[5][x][z])
                        needsDraw[x][y][z] |= Side::BOTTOM;
                }
            }
        }
        needsMeshUpdate = true;
        return;
    }

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                if (getVoxel(x, y, z) == VOXEL_NONE)
                    continue;

                if ((z == CHUNK_SIZE - 1 && !obstructions[0][x][y]) || (z != CHUNK_SIZE - 1 && getVoxel(x, y, z + 1) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::FRONT;
                if ((z == 0 && !obstructions[1][x][y]) || (z != 0 && getVoxel(x, y, z - 1) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::BACK;
                if ((x == 0 && !obstructions[2][y][z]) || (x != 0 && getVoxel(x - 1, y, z) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::LEFT;
                if ((x == CHUNK_SIZE - 1 && !obstructions[3][y][z]) || (x != CHUNK_SIZE - 1 && getVoxel(x + 1, y, z) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::RIGHT;
                if ((y == CHUNK_SIZE - 1 && !obstructions[4][x][z]) || (y != CHUNK_SIZE - 1 && getVoxel(x, y + 1, z) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::TOP;
                if ((y == 0 && !obstructions[5][x][z]) || (y != 0 && getVoxel(x, y - 1, z) == VOXEL_NONE))
                    needsDraw[x][y][z] |= Side::BOTTOM;
            }
        }
    }

    needsMeshUpdate = true;
}

void Chunk::getObstructions(Side side, bool obstructions[CHUNK_SIZE][CHUNK_SIZE])
{
    if (side == Side::NONE)
        return;

    if (isSimpleChunk)
    {
        bool obstructed = simpleChunkVoxel != VOXEL_NONE;
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                obstructions[i][j] = obstructed;
            }
        }
        return;
    }
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            bool obstructed = false;
            switch (side)
            {
            case Side::FRONT: // Z = CHUNK_SIZE - 1
                obstructed = voxels[i][j][CHUNK_SIZE - 1] != VOXEL_NONE;
                break;
            case Side::BACK: // Z = 0
                obstructed = voxels[i][j][0] != VOXEL_NONE;
                break;

            case Side::LEFT: // X = 0
                obstructed = voxels[0][i][j] != VOXEL_NONE;
                break;

            case Side::RIGHT: // X = CHUNK_SIZE - 1
                obstructed = voxels[CHUNK_SIZE - 1][i][j] != VOXEL_NONE;
                break;

            case Side::TOP: // Y = CHUNK_SIZE - 1
                obstructed = voxels[i][CHUNK_SIZE - 1][j] != VOXEL_NONE;
                break;

            case Side::BOTTOM: // Y = 0
                obstructed = voxels[i][0][j] != VOXEL_NONE;
                break;

            default:
                break;
            }

            obstructions[i][j] = obstructed;
        }
    }
}

void Chunk::generateMesh()
{
    needsMeshUpdate = false;
    needsMeshUpload = false;

    meshVertices.clear();
    meshNormals.clear();
    meshColors.clear();

    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            for (int k = 0; k < CHUNK_SIZE; k++)
            {
                if (getVoxel(i, j, k) == VOXEL_NONE)
                    continue;

                std::vector<float> faces_vec = CubeMeshSides::faces_at(i, j, k, needsDraw[i][j][k]);
                meshVertices.insert(meshVertices.end(), faces_vec.begin(), faces_vec.end());

                for (int l = 0; l < faces_vec.size() / 3; l++)
                {
                    meshColors.push_back(getVoxel(i, j, k));
                }
            }
        }
    }

    meshSize = meshVertices.size() / 3;

    needsMeshUpload = true;
}

void Chunk::uploadMesh()
{
    needsMeshUpload = false;
    // Check if the VAO, VBO and EBO are initialized
    if (!hasBuffer)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // glGenBuffers(1, &EBO);

        hasBuffer = true;
    }
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Make room for 3 floats coords and 1 int color per vertex
    glBufferData(GL_ARRAY_BUFFER, meshSize * (3 * sizeof(float) + sizeof(int)), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, meshSize * 3 * sizeof(float), meshVertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, meshSize * 3 * sizeof(float), meshSize * sizeof(int), meshColors.data());

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void *)(meshSize * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Chunk::discard()
{
    if (hasBuffer)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        // glDeleteBuffers(1, &EBO);
        hasBuffer = false;
    }

    scheduledForDeletion = true;
}

void Chunk::draw(Shader *shader)
{
    if (isEmpty())
        return;

    if (meshSize == 0)
        return;

    if (!hasBuffer || scheduledForDeletion)
        return;

    if (isSimpleChunk && simpleChunkVoxel == VOXEL_NONE)
        return;

    shader->use();
    shader->setMat4("model", m_modelMatrix);
    shader->setMat4("invModel", m_invModelMatrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);
    glBindVertexArray(0);
}

void Chunk::print_info()
{
    log_debug("Chunk (%d, %d, %d)", m_x, m_y, m_z);
    log_debug("  isSimpleChunk: %s", isSimpleChunk ? "true" : "false");
    log_debug("  simpleChunkVoxel: %d", simpleChunkVoxel);
    log_debug("  meshSize: %d", meshSize);
    log_debug("  VAO: %d, VBO: %d, EBO: %d", VAO, VBO, EBO);
    log_debug("  Position: %d %d %d", m_x, m_y, m_z);
    log_debug("  Edge changed: %d", edgeChanged);
    log_debug("  Needs side occlusion update: %s", needsSideOcclusionUpdate ? "true" : "false");
    log_debug("  Needs mesh update: %s", needsMeshUpdate ? "true" : "false");
    log_debug("  Needs mesh upload: %s", needsMeshUpload ? "true" : "false");
    log_debug("  Scheduled for deletion: %s", scheduledForDeletion ? "true" : "false");
    log_debug("  m_modelMatrix: %f %f %f %f", m_modelMatrix[0][0], m_modelMatrix[0][1], m_modelMatrix[0][2], m_modelMatrix[0][3]);
    log_debug("                 %f %f %f %f", m_modelMatrix[1][0], m_modelMatrix[1][1], m_modelMatrix[1][2], m_modelMatrix[1][3]);
    log_debug("                 %f %f %f %f", m_modelMatrix[2][0], m_modelMatrix[2][1], m_modelMatrix[2][2], m_modelMatrix[2][3]);
    log_debug("                 %f %f %f %f", m_modelMatrix[3][0], m_modelMatrix[3][1], m_modelMatrix[3][2], m_modelMatrix[3][3]);
    log_debug("  m_invModelMatrix: %f %f %f %f", m_invModelMatrix[0][0], m_invModelMatrix[0][1], m_invModelMatrix[0][2], m_invModelMatrix[0][3]);
    log_debug("                    %f %f %f %f", m_invModelMatrix[1][0], m_invModelMatrix[1][1], m_invModelMatrix[1][2], m_invModelMatrix[1][3]);
    log_debug("                    %f %f %f %f", m_invModelMatrix[2][0], m_invModelMatrix[2][1], m_invModelMatrix[2][2], m_invModelMatrix[2][3]);
    log_debug("                    %f %f %f %f", m_invModelMatrix[3][0], m_invModelMatrix[3][1], m_invModelMatrix[3][2], m_invModelMatrix[3][3]);
}