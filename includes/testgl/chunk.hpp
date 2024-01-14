#pragma once

#include "testgl/constants.hpp"
#include "testgl/voxel.hpp"
#include "learnopengl/Shaders.hpp"
#include "testgl/cube.hpp"
#include "testgl/worldgen.hpp"

#include <glm/glm.hpp>

class Chunk
{
private:
    Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    Sides needsDraw[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    std::vector<float> meshVertices;
    std::vector<float> meshNormals; // unused
    std::vector<int> meshColors;
    std::vector<unsigned int> meshIndices;
    int meshSize;

    unsigned int VAO, VBO, EBO;

    int m_x, m_y, m_z;

    glm::mat4 m_modelMatrix;
    glm::mat4 m_invModelMatrix;

    bool hasBuffer, needsSideOcclusionUpdate, needsMeshUpdate, needsMeshUpload;
    Sides edgeChanged;

public:
    Chunk() = default;
    // Coordinates of the chunk in the world (in chunks)
    Chunk(int x, int y, int z);
    ~Chunk();

    Voxel getVoxel(int x, int y, int z);
    void setVoxel(int x, int y, int z, Voxel value);
    void setVoxelLayer(int y, Voxel value);

    void populate(WorldGenerator::function_t worldGenerator);

    // For proper culling with neighboring chunks
    void getObstructions(Side side, bool(obstructions)[CHUNK_SIZE][CHUNK_SIZE]);

    void calculateNeedsDraw();
    void generateMesh();
    void uploadMesh();
    void draw(Shader *shader);
    void discard();

    glm::mat4 getModelMatrix() { return m_modelMatrix; }
    glm::mat4 getInvModelMatrix() { return m_invModelMatrix; }
    int getX() { return m_x; }
    int getY() { return m_y; }
    int getZ() { return m_z; }
    ChunkPos getPos() { return ChunkPos(m_x, m_y, m_z); }

    bool getNeedsSideOcclusionUpdate() { return needsSideOcclusionUpdate; }
    bool getNeedsMeshUpdate() { return needsMeshUpdate; }
    bool getNeedsMeshUpload() { return needsMeshUpload; }
    Sides getEdgeChanged() { return edgeChanged; }
};
