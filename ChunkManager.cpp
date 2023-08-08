#include "ChunkManager.hpp"
#include "Biomes.hpp"

#include <FastNoise/FastNoise.h> // Noise generator
#include <vector> // For std::vector
#include <cmath> // Sqrt and pow
#include <glm/gtx/vector_angle.hpp> // glm::rotate



// Generate all of our chunks
void ChunkManager::GenerateChunks()
{
    if(World::seedLogging) // If we have seed logging enabled, print out the seed
        std::cout << "World seed: " << seed << std::endl;
    srand(seed); // Seed random number generator with our world seed

    // Initialize FastNoise2
    auto OpenSimplex = FastNoise::New<FastNoise::OpenSimplex2>();
    auto FractalFBm = FastNoise::New<FastNoise::FractalFBm>();
    FractalFBm->SetSource(OpenSimplex);
    FractalFBm->SetGain(2.080f);
    FractalFBm->SetOctaveCount(4);
    FractalFBm->SetLacunarity(0.15f);

    auto DomainScale = FastNoise::New<FastNoise::DomainScale>();
    DomainScale->SetSource(FractalFBm);
    DomainScale->SetScale(45.646f);

    auto PositionOutput = FastNoise::New<FastNoise::PositionOutput>();
    PositionOutput->Set<FastNoise::Dim::Y>(0.072); // This changes the size of biomes. Low values = big

    auto add = FastNoise::New<FastNoise::Add>();
    add->SetLHS(DomainScale);
    add->SetRHS(PositionOutput);

    // Vector structure to hold our noise
    std::vector<float> noiseOutput((chunkDiameter * 2 + 1) * (chunkDiameter * 2 + 1));
    // Index for when we loop through our noiseOutput 
    GLint noiseIndex = 0;

    // Generate 2D heightmap noise
    if(World::chunkDiameter > 0 )
        add->GenUniformGrid2D(noiseOutput.data(), 0, 0, chunkDiameter * 2 + 1, chunkDiameter * 2 + 1, 0.0015f, seed);

    GLuint noise = 0;
    GLfloat heightMax = 64.0f;
    GLfloat heightMin = 1.0f;

    // Generate our chunks if they have not been generated yet
    if(chunks_.empty())
    {
        for(GLint z = -1 * chunkDiameter; z <= chunkDiameter; z++)
        {
            for(GLint x = -1 * chunkDiameter; x <= chunkDiameter; x++)
            {
                // Set our new 2D noise
                if(World::chunkDiameter > 0 )
                    noise = ((GLint)((abs(noiseOutput[noiseIndex++]) + 1) / 2 * (heightMax - heightMin) + heightMin) % ((GLint)heightMax - 1) + 1) % (sizeof(BiomeConfiguration) / sizeof(Biome_Configuration) - 1); // mod biome count
                for(GLint y = 0; y < World::chunksTall; y++)
                {
                    glm::vec3 position = glm::vec3(x, y, z);
                    if(World::randomBiomeGenerationPerChunk)
                        chunks_[position] = new Chunk(x, y, z, rand() % (sizeof(BiomeConfiguration) / sizeof(Biome_Configuration)));
                    else
                        chunks_[position] = new Chunk(x, y, z, noise);
                }
            }
        }


        // Loop through our chunks and generate their blocks, passing in the 
        // biomes on adjacent chunks
        for(GLint z = -1 * chunkDiameter; z <= chunkDiameter; z++)
        {
            for(GLint x = -1 * chunkDiameter; x <= chunkDiameter; x++)
            {
                for(GLint y = 0; y < World::chunksTall; y++)
                {
                    glm::vec3 position = glm::vec3(x, y, z);
                    // Get the biomes for adjacent chunks
                    GLint biomeTypeIDPosX = (x+1 <= chunkDiameter ? chunks_[glm::vec3(x+1, y, z)]->biomeID : Biomes::Null_Biome);
                    GLint biomeTypeIDPosZ = (z+1 <= chunkDiameter ? chunks_[glm::vec3(x, y, z+1)]->biomeID : Biomes::Null_Biome);
                    GLint biomeTypeIDNegX = (x-1 >= -1 * chunkDiameter ? chunks_[glm::vec3(x-1, y, z)]->biomeID : Biomes::Null_Biome);
                    GLint biomeTypeIDNegZ = (z-1 >= -1 * chunkDiameter ? chunks_[glm::vec3(x, y, z-1)]->biomeID : Biomes::Null_Biome);
                    chunks_[position]->GenerateBlocks(seed, biomeTypeIDPosX, biomeTypeIDPosZ, biomeTypeIDNegX, biomeTypeIDNegZ);
                }
            }
        }
    }
}



// Returns area of triangle given 3 points
GLfloat area(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3)
{
    return abs((pos1.x*(pos2.z-pos3.z) + pos2.x*(pos3.z-pos1.z)+ pos3.x*(pos1.z-pos2.z))/2.0);
}



// Render all of our generated chunks
void ChunkManager::RenderChunks(glm::vec3 cameraPosition, glm::vec3 cameraOrientation, GLuint cubeShaderProgramID)
{
    // Only do the frustum culling math if the cameraPosition 
    // and cameraOrientation has actually moved from the previous frame
    if(previousCameraPosition != cameraPosition || previousCameraOrientation != cameraOrientation)
    {
        // Make camera position 10 blocks behind what it actually is
        GLfloat posAdjustmentAmount = World::blockSize * World::chunkSize;
        cameraPosition.x = (-1 * cameraOrientation.x) * posAdjustmentAmount + cameraPosition.x;
        cameraPosition.z = (-1 * cameraOrientation.z) * posAdjustmentAmount + cameraPosition.z;

        GLfloat rotateAngle = 70.0f;
        // // If a chunk is closer than this distance, we will render it even if its behind us
        GLfloat minRenderDistance = World::chunkSize * World::blockSize;//(World::chunkSize / (World::blockSize > 1 ? World::blockSize : (1 / World::blockSize))) * 2 * (World::blockSize > 1 ? World::blockSize : (1 / World::blockSize)); 

        cameraOrientation = glm::normalize(cameraOrientation);
        glm::vec3 cameraRotatedHorizontalPositive = glm::rotate(cameraOrientation, glm::radians(rotateAngle), glm::vec3(0, 1, 0));
        glm::vec3 cameraRotatedHorizontalNegative = glm::rotate(cameraOrientation, glm::radians(-rotateAngle), glm::vec3(0, 1, 0));

        GLfloat pointOffset = World::BlockRenderDistance * 3;
        glm::vec3 triPoint1 = glm::vec3(pointOffset * cameraRotatedHorizontalPositive.x + cameraPosition.x, 0, pointOffset * cameraRotatedHorizontalPositive.z + cameraPosition.z);
        glm::vec3 triPoint2 = glm::vec3(pointOffset * cameraRotatedHorizontalNegative.x + cameraPosition.x, 0, pointOffset * cameraRotatedHorizontalNegative.z + cameraPosition.z);

        GLfloat totalArea = area(cameraPosition, triPoint1, triPoint2);

        glm::vec3 chunkPosition = glm::vec3(0.0f, 0.0f, 0.0f);

        // Distance from chunk to camera
        GLfloat distanceBottomLeftCorner = 0.0f;
        GLfloat distanceTopLeftCorner = 0.0f;
        GLfloat distanceBottomRightCorner = 0.0f;
        GLfloat distanceTopRightCorner = 0.0f;
        // Render all of our chunks
        for(GLint x = -1 * chunkDiameter; x <= chunkDiameter; x++)
        {
            for(GLint z = -1 * chunkDiameter; z <= chunkDiameter; z++)
            {
                for(GLint y = 0; y < World::chunksTall; y++)
                {
                    // Check if this chunk is in viewing frustum
                    // Use distance formula to see if chunk in render distance
                    chunkPosition = glm::vec3((GLfloat)x * (GLfloat)World::chunkSize * World::blockSize, (GLfloat)y * (GLfloat)World::chunkSize * World::blockSize, (GLfloat)z * (GLfloat)World::chunkSize * World::blockSize);
                    distanceBottomLeftCorner = sqrt(pow(chunkPosition.x - cameraPosition.x, 2) + pow(chunkPosition.z - cameraPosition.z, 2));
                    distanceTopLeftCorner = sqrt(pow(chunkPosition.x - cameraPosition.x, 2) + pow((chunkPosition.z) + ((GLfloat)World::chunkDepthZ * World::blockSize) - World::blockSize - cameraPosition.z, 2));
                    distanceBottomRightCorner = sqrt(pow((chunkPosition.x) + ((GLfloat)World::chunkWidthX * World::blockSize) - World::blockSize - cameraPosition.x, 2) + pow(chunkPosition.z - cameraPosition.z, 2));
                    distanceTopRightCorner = sqrt(pow((chunkPosition.x) + ((GLfloat)World::chunkWidthX * World::blockSize) - World::blockSize - cameraPosition.x, 2) + pow(chunkPosition.z + ((GLfloat)World::chunkDepthZ * World::blockSize) - World::blockSize - cameraPosition.z, 2));

                    // If the distance from the camera to the chunk is less than our minimum distance, 
                    // we want to always render it even if the chunk is behind us
                    if(distanceBottomLeftCorner <= minRenderDistance || distanceBottomRightCorner <= minRenderDistance || distanceTopLeftCorner <= minRenderDistance || distanceTopRightCorner <= minRenderDistance)
                    {
                        chunks_[glm::vec3(x, y, z)]->RenderChunk(cubeShaderProgramID, true);
                        chunks_[glm::vec3(x, y, z)]->shouldRender = true;
                        chunksPassed++;
                    }
                    else if(distanceBottomLeftCorner <= World::BlockRenderDistance || distanceBottomRightCorner <= World::BlockRenderDistance || distanceTopLeftCorner <= World::BlockRenderDistance || distanceTopRightCorner <= World::BlockRenderDistance)
                    {
                        // Check if our point is inside triangle
                        glm::vec3 point = glm::vec3(x * (World::chunkWidthX * World::blockSize), 0, z * (World::chunkDepthZ * World::blockSize));
                        GLfloat totalPointArea = area(cameraPosition, triPoint2, point) + area(cameraPosition, triPoint1, point) + area(triPoint2, triPoint1, point);

                        if(abs(totalPointArea - totalArea) <= 5)
                        {
                            chunks_[glm::vec3(x, y, z)]->RenderChunk(cubeShaderProgramID, true);
                            chunks_[glm::vec3(x, y, z)]->shouldRender = true;
                            chunksPassed++;
                        }
                        else
                        {
                            chunks_[glm::vec3(x, y, z)]->shouldRender = false;
                            chunksFailed++;
                        }
                    }
                    else
                    {
                        chunks_[glm::vec3(x, y, z)]->shouldRender = false;
                        chunksFailed++;
                    }
                }
            }
        }
        previousCameraPosition = cameraPosition;
        previousCameraOrientation = cameraOrientation;
    }
    else // Run this code if the camera orientation and position has not changed from previous frame
    {
        for(GLint x = -1 * chunkDiameter; x <= chunkDiameter; x++)
        {
            for(GLint z = -1 * chunkDiameter; z <= chunkDiameter; z++)
            {
                for(GLint y = 0; y < World::chunksTall; y++)
                {
                    chunks_[glm::vec3(x, y, z)]->RenderChunk(cubeShaderProgramID, true);
                }
            }
        }
    }

    // Render transparent
    for(GLint x = -1 * chunkDiameter; x <= chunkDiameter; x++)
    {
        for(GLint z = -1 * chunkDiameter; z <= chunkDiameter; z++)
        {
            for(GLint y = 0; y < World::chunksTall; y++)
            {
                if(chunks_[glm::vec3(x, y, z)]->shouldRender)
                {
                    chunks_[glm::vec3(x, y, z)]->RenderChunk(cubeShaderProgramID, false);
                }
            }
        }
    }
    // If our frustum culling logging is enabled, std::cout our results
    if(World::frustumCullingLogging)
    {
        std::cout << "Chunks Passed: " << chunksPassed << " Chunks Failed: " << chunksFailed << std::endl;
        chunksFailed = 0;
        chunksPassed = 0;
    }
    else
    {
        chunksFailed = 0;
        chunksPassed = 0;
    }
}


