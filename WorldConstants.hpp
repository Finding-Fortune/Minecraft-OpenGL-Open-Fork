#pragma once

#include <glad/glad.h>
#include <cmath>
#include <glm/glm.hpp>



// Configurable world variables
namespace World
{
    /* Block Settings */
    const GLfloat blockSize = pow(2, 0);  // Size of each block in world units
                                           // Its a power of 2, as they do not have binary imprecision

    /* Chunk Settings */
    const GLuint chunkSize     = 32;
    const GLuint chunkWidthX   = chunkSize;  // How many blocks wide a chunk is
    const GLuint chunkHeightY  = chunkSize;  // How many blocks tall a chunk is
    const GLuint chunkDepthZ   = chunkSize;  // How many blocks deep a chunk is
    const GLuint chunkVolume   = chunkWidthX * chunkHeightY * chunkDepthZ; // How many blocks a chunk is
    const GLuint chunkDiameter = 12;  // The amount of chunks generated across. Basically the render distance
                                     // If 0 then 1 chunks generate, If 1 then 9 chunks generate (3x3), if 2 then 25 chunks generate (5x5), etc
    const GLfloat BlockRenderDistance = 40 * chunkSize * blockSize; // Will render chunks within n blocks

    /* World Settings */
    const GLuint chunksTall     = 1;     // How many chunks tall we generate the world / world height limit
    const GLuint heightLimit    = 64;    // Max height limit of the world
    const GLboolean randomSeed  = false; // If true then the seed will always be 100, otherwise random
    const GLuint defaultSeed = 926797;    // The default seed if randomSeed is false
    const GLboolean randomBiomeGenerationPerChunk = false; // If true then biomes generate randomly per chunk, otherwise generates with noise
    const GLuint waterLevel = 0;         // The height of the water level

    /* Logging */
    const GLboolean frustumCullingLogging = false; // If true then we log the amount of passed and failed chunks in the frustum culling test in ChunkManager.cpp
    const GLboolean seedLogging = false;           // If true then we print out the seed on world load

    /* GUI Settings */
    const GLfloat crosshairThickness = 0.003f; // Thickness of the crosshair lines
    const GLfloat crosshairLength = 0.025f;    // Length of the crosshair lines

    /* Environment Settings */
    inline glm::vec4 skyColor = glm::vec4(0.54f, 0.81f, 0.94f, 1.0f); // The angle of the sun in degrees

    /* Lighting Setting */
    const GLboolean ambientOcclusionEnabled = true; // Whether ambient occlusion for block faces is enabled

    /* Player Settings */
    const GLfloat blockBreakingSpeed = 0.1f; // How fast the player breaks blocks per second
    const GLuint playerReachScaleAmount = 500; // How many steps we take to place a block
    const GLfloat blockPlacingSpeed = 0.1f; // How fast the player places blocks per second
}


