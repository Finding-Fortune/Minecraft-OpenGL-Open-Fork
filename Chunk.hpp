#pragma once

#include "Block.hpp"
#include "Biomes.hpp"
#include "WorldConstants.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL // Need this to use the glm hashes
#include "glm/gtx/hash.hpp" // Include hash maps for unordered_map
#include <unordered_map> // For unordered_map
#include <iostream>
#include <vector> // For std::vector



class Chunk
{
public:
    // Whether we should render the chunk
    GLboolean shouldRender = false;
    // Offset of our chunk
    GLfloat offset_x;
    GLfloat offset_z;
    GLfloat offset_y;
    // Position of our chunk
    GLint chunk_position_x;
    GLint chunk_position_y;
    GLint chunk_position_z;
    // 1D array with the chunk dimensions
    Block chunk[World::chunkWidthX * World::chunkHeightY * World::chunkDepthZ];
    // Min and max height of a chunk
    GLfloat heightMin = 1.0f;
    GLfloat heightMax = World::heightLimit;

    // Which biome ID this chunk is
    GLuint biomeID = 0;
    // A check for whether this chunk has its mesh created
    GLboolean meshCreated = false;

    // Constructor with positions of chunk passed in
    Chunk(GLint position_x, GLint position_y, GLint position_z, GLuint BiomeIndex);
    ~Chunk();
    // Generate our full chunk
    void GenerateBlocks(GLuint seed, GLint biomeTypeIDPosX, GLint biomeTypeIDPosZ, GLint biomeTypeIDNegX, GLint biomeTypeIDNegZ);
    // Now that we have our model, actually send the geometry/mesh/batch to GPU
    void RenderChunk(GLuint cubeShaderProgramID, GLboolean renderOpaque);
    // Get block in 1D chunk array
    Block GetBlock(GLint x, GLint y, GLint z);
    Block GetBlock(glm::vec3 position);
    // Set the block type for a block
    void SetBlockType(glm::vec3 position, GLint BlockTypeID);
    // Remesh our chunk
    void RebuildMesh();

private:
    // All the buffers for opaque blocks for our chunk
    VAO ChunkOpaqueVAO;
    VBO ChunkOpaqueVBO;
    // All the buffers for transparent blocks for our chunk
    VAO ChunkTransparentVAO;
    VBO ChunkTransparentVBO;
    // Our chunk vertices and indices to send to GPU/shader
    std::vector<GLuint> chunkOpaqueVertices;
    std::vector<GLuint> chunkTransparentVertices;

    // Determine block ambient occlusion
    void DetermineAOTopFace(glm::vec3 position);
    void DetermineAOFrontFace(glm::vec3 position);
    void DetermineAOBackFace(glm::vec3 position);
    void DetermineAOLeftFace(glm::vec3 position);
    void DetermineAORightFace(glm::vec3 position);
    // Minify full block chunk into only faces that need render (a mesh)
    void RenderMesh();
    /* Utility methods for 1D chunk array */
    // Set block in 1D chunk array
    void SetBlock(Block block);
    // Set position of block in 1D chunk array
    void SetPosition(GLint x, GLint y, GLint z);
    // Check if a block is air in 1D chunk array
    GLboolean IsTransparent(GLint x, GLint y, GLint z);
    GLboolean IsTransparent(glm::vec3 position);
    GLboolean IsFoliage(GLint x, GLint y, GLint z);
    GLboolean IsFoliage(glm::vec3 position);
    // Formula for checking 1D chunk block is: chunk[x + z*World::chunkWidthX + y*World::chunkWidthX*World::chunkDepthZ]

    // Draw a single face of a block
    void DrawFace(Block block, GLuint faceIndex);
};

// Keep track of where each chunk is located with hash map
inline std::unordered_map<glm::vec3, Chunk *> chunks_; // A hash map of chunks, where the key is a vec2


