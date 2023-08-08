#pragma once

#include "WorldConstants.hpp"
#include "Chunk.hpp"

#include <stdlib.h> // For generating random numbers



class ChunkManager
{
public:
    GLint chunkDiameter = World::chunkDiameter; // This is square root of chunkAmount
    // Keep track of where each chunk is located with hash map
    // std::unordered_map<glm::vec3, Chunk *> chunks_; // A hash map of chunks, where the key is a vec2

    GLint max_chunks_x;
    GLint max_chunks_z;

    GLuint seed = World::randomSeed ? rand() % 1000000 : World::defaultSeed; // The seed for the noise generator    

    // Empty constructor
    ChunkManager(){};

    // Generate the chunks
    void GenerateChunks();
    // Once chunks are generated, render them
    void RenderChunks(glm::vec3 cameraPosition, glm::vec3 cameraOrientation, GLuint cubeShaderProgramID);

private:
    glm::vec3 previousCameraPosition;
    glm::vec3 previousCameraOrientation;
    GLuint chunksPassed = 0;
    GLuint chunksFailed = 0;
};


