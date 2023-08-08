#include "Chunk.hpp"

#include <FastNoise/FastNoise.h> // Noise generator
#include <cmath> // for abs()
#include <vector> // For std::vector
#include <stdlib.h> // For generating random numbers
#include <algorithm> // std::find() function
#include <string> // For std::string



Chunk::Chunk(GLint position_x, GLint position_y, GLint position_z, GLuint BiomeIndex) : ChunkOpaqueVAO(), ChunkOpaqueVBO()
{
    // Generate Vertex Array Object and binds it
    /* Opaque VBO */
	ChunkOpaqueVAO.Bind();
    ChunkOpaqueVBO.Bind();
	// Links VBO attributes such as coordinates and colors to VAO
	ChunkOpaqueVAO.LinkAttrib(ChunkOpaqueVBO, 0, 1, GL_FLOAT, 1 * sizeof(GLuint), (void*)0);
	// Unbind all to prevent accidentally modifying them
	ChunkOpaqueVBO.Unbind();
    ChunkOpaqueVAO.Unbind();

    /* Transparent VBO */
    ChunkTransparentVAO.Bind();
    ChunkTransparentVBO.Bind();
	// Links VBO attributes such as coordinates and colors to VAO
	ChunkOpaqueVAO.LinkAttrib(ChunkTransparentVBO, 0, 1, GL_FLOAT, 1 * sizeof(GLuint), (void*)0);
	// Unbind all to prevent accidentally modifying them
    ChunkTransparentVAO.Unbind();
	ChunkTransparentVBO.Unbind();

    // Set offset of our chunk, so that chunks load at different positions
    // (not on top of each other)
    offset_x = position_x * (World::chunkWidthX  * World::blockSize);
    offset_y = position_y * (World::chunkHeightY * World::blockSize);
    offset_z = position_z * (World::chunkDepthZ  * World::blockSize);
    // Position of our chunk
    chunk_position_z = position_z;
    chunk_position_y = position_y;
    chunk_position_x = position_x;

    // Set our biome
    biomeID = BiomeIndex;
}



Chunk::~Chunk()
{
    // Delete all buffers after program is stopped
    // std::cout << "Deleting Chunk Buffers" << std::endl;
    ChunkOpaqueVAO.Delete();
    ChunkOpaqueVBO.Delete();
    ChunkTransparentVAO.Delete();
    ChunkTransparentVBO.Delete();
    delete[] &chunkOpaqueVertices;
    delete[] &chunkTransparentVertices;
    delete[] &chunk;
}



void Chunk::GenerateBlocks(GLuint seed, GLint biomeTypeIDPosX, GLint biomeTypeIDPosZ, GLint biomeTypeIDNegX, GLint biomeTypeIDNegZ)
{
    // Initialize FastNoise2
    auto OpenSimplex = FastNoise::New<FastNoise::OpenSimplex2>();
    auto FractalFBm = FastNoise::New<FastNoise::FractalFBm>();
    FractalFBm->SetSource(OpenSimplex);
    FractalFBm->SetGain(BiomeConfiguration[biomeID].NoiseGain);
    FractalFBm->SetOctaveCount(4);
    FractalFBm->SetLacunarity(5.0f);
    // FractalFBm->SetWeightedStrength(0.01f);
    
    auto DomainScale = FastNoise::New<FastNoise::DomainScale>();
    DomainScale->SetSource(FractalFBm);
    DomainScale->SetScale(0.196f);

    auto PositionOutput = FastNoise::New<FastNoise::PositionOutput>();
    PositionOutput->Set<FastNoise::Dim::Y>(0.05f);

    auto add = FastNoise::New<FastNoise::Add>();
    add->SetLHS(DomainScale);
    add->SetRHS(PositionOutput);

    // Vector structure to hold our noise
    std::vector<GLfloat> interpolNoise(9); // Top center
    
     std::vector<GLfloat> noiseOutput(World::chunkWidthX * World::chunkDepthZ);
    // Index for when we loop through our noiseOutput 
    GLint noiseIndex = 0;

    GLint adjustedChunkPosX = chunk_position_x * (GLint)World::chunkWidthX;
    GLint adjustedChunkPosZ = chunk_position_z * (GLint)World::chunkDepthZ;
    // Generate a chunkWidthX x chunkDepthZ area of noise
    add->GenUniformGrid2D(noiseOutput.data(), adjustedChunkPosX, adjustedChunkPosZ, (GLint)World::chunkWidthX, (GLint)World::chunkDepthZ, BiomeConfiguration[biomeID].NoiseFrequency, seed);

    GLfloat cubesY; // Variable to store our noise in the for loops

    GLuint treeHeightIndex = 0; // Index for seeing the max tree height

    for(GLfloat z = 0; z < World::chunkDepthZ; z += 1)
    for(GLfloat x = 0; x < World::chunkWidthX; x += 1)
    {
        cubesY = (GLint)(abs(noiseOutput[noiseIndex]) * (heightMax - heightMin) + heightMin);
        noiseIndex++;
        

        for(GLfloat y = 0; y < World::chunkHeightY; y += 1)
        {
            // If y is less than cubesY, we want to generate the block,
            // because cubesY is our max height
            if(y + offset_y < cubesY)
            {
                Block tempBlock;
                // Set our block's position with the correct chunk offset
                tempBlock.position = glm::vec3(x, y, z);

                // Logic for determining which block type to place
                if(y + offset_y >= cubesY - 1)
                    tempBlock.blockTypeID = BiomeConfiguration[biomeID].Surface_Block;
                else if(y + offset_y >= cubesY - 4)
                    tempBlock.blockTypeID = BiomeConfiguration[biomeID].Subsurface_Block;
                else
                    tempBlock.blockTypeID = blocks["Stone_Block"]["index"];;

                // Set our block
                SetBlock(tempBlock);
            }
            // If its greater than cubesY, we don't want to generate the block
            // but we still want to record its position
            else
            {
                // Random chance to make an oak tree
                if((y + offset_y == cubesY && rand() % BiomeConfiguration[biomeID].TreeFrequency == 0 && BiomeConfiguration[biomeID].TreeFrequency != -1) || (treeHeightIndex >= 1 && treeHeightIndex <= 5))
                {
                    // Check if this tree would even fit inside the chunk (for now 
                    // don't want to deal with placing leaves across chunks)
                    if(((treeHeightIndex == 0 && y + 7.0f < World::chunkHeightY) || treeHeightIndex > 0) && x + 2 < World::chunkWidthX && z + 2 < World::chunkDepthZ and x > 1 and z > 1)
                    {
                        Block woodBlock;
                        // Set our block's position with the correct chunk offset
                        woodBlock.position = glm::vec3(x, y, z);
                        woodBlock.blockTypeID = blocks["Oak_Log"]["index"];
                        // Set our block
                        SetBlock(woodBlock);
                        treeHeightIndex = (treeHeightIndex == -1 ? 1 : treeHeightIndex + 1);

                        // Put leaves around wood
                        if(treeHeightIndex == 6)
                        {
                            Block leavesBlock;
                            leavesBlock.position = glm::vec3(x, y+1, z);
                            leavesBlock.blockTypeID = blocks["Oak_Leaves"]["index"];
                            // Make first layer of leaves
                            for(int i=x-2; i<=x+2; i++)
                                for(int j=z-2; j<=z+2; j++)
                                {
                                    leavesBlock.position = glm::vec3(i, y+1, j);
                                    SetBlock(leavesBlock);
                                }

                            // Make second layer of leaves
                            for(int i=x-2; i<=x+2; i++)
                                for(int j=z-2; j<=z+2; j++)
                                    if((i != x+2 && j != z+2) && (i != x-2 && j != z+2) && (i != x+2 && j != z-2) && (i != x-2 && j != z-2))
                                    {
                                        leavesBlock.position = glm::vec3(i, y+2, j);
                                        SetBlock(leavesBlock);
                                    }
                        }
                    }
                }
                else // If we are not making a tree then just record its position
                {
                    // If below a certain threshold, put water instead of air
                    if(y == World::waterLevel && BiomeConfiguration[biomeID].AllowWater) // Water level check
                    {
                        Block waterBlock;
                        // Set our block's position with the correct chunk offset
                        waterBlock.position = glm::vec3(x, y, z);
                        waterBlock.blockTypeID = (BiomeConfiguration[biomeID].HotTemperature == true ? blocks["Water"]["index"] : blocks["Ice_Block"]["index"]);
                        // Set our block
                        SetBlock(waterBlock);
                    }
                    else
                    {
                        SetPosition(x, y, z);
                    }
                }
            }
        } // End of for loop for y
        treeHeightIndex = 0;
    }
}



void Chunk::DetermineAOTopFace(glm::vec3 position)
{
    /* Determine top face AO */
    // If we are not on an x-axis chunk border
    if(position.x != 0 && position.x < World::chunkWidthX - 1 && position.y < World::chunkHeightY - 1)
    {
        // Check if there is a block over and up 1 to the left
        if(!IsTransparent(position.x + 1, position.y + 1, position.z) || IsFoliage(position.x + 1, position.y + 1, position.z))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopRight = true;
            tempBlock.topFace.aoTopLeft = true;
            SetBlock(tempBlock);
        }
        if(!IsTransparent(position.x - 1, position.y + 1, position.z) || IsFoliage(position.x - 1, position.y + 1, position.z))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomLeft = true;
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    if(position.x == 0 && position.y < World::chunkHeightY - 1) // We are on a x axis chunk border
    {
        if(abs(chunk_position_x - 1) <= World::chunkDiameter) // Check out of chunk
        {
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsTransparent(World::chunkWidthX-1, position.y+1, position.z) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsFoliage(World::chunkWidthX-1, position.y+1, position.z))
            {
                // We want to do ambient occlusion bottom left and bottom right
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoBottomLeft = true;
                tempBlock.topFace.aoBottomRight = true;
                SetBlock(tempBlock);                                    
            }
            
        }
        // Check in the same chunk
        if(!IsTransparent(position.x + 1, position.y + 1, position.z) || IsFoliage(position.x + 1, position.y + 1, position.z))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopLeft = true;
            tempBlock.topFace.aoTopRight = true;
            SetBlock(tempBlock);
        }
    }
    if(position.x == World::chunkWidthX - 1 && position.y < World::chunkHeightY - 1) // We are on a x axis chunk border
    {
        if(abs(chunk_position_x + 1) <= World::chunkDiameter) // Check out of chunk
        {
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsTransparent(0, position.y+1, position.z) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsFoliage(0, position.y+1, position.z))
            {
                // We want to do ambient occlusion bottom left and bottom right
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopRight = true;
                tempBlock.topFace.aoTopLeft = true;
                SetBlock(tempBlock);
            }
        }
        // Check in the same chunk
        if(!IsTransparent(position.x - 1, position.y + 1, position.z) || IsFoliage(position.x - 1, position.y + 1, position.z))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomLeft = true;
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }

    // If we are not on a z-axis chunk border
    if(position.z != 0 && position.z < World::chunkDepthZ - 1 && position.y < World::chunkHeightY - 1)
    {
        // Check if there is a block over and up 1 to the left
        if(!IsTransparent(position.x, position.y + 1, position.z - 1) || IsFoliage(position.x, position.y + 1, position.z - 1))
        {
            // We want to do ambient occlusion top left and bottom left
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopLeft = true;
            tempBlock.topFace.aoBottomLeft = true;
            SetBlock(tempBlock);
        }
        if(!IsTransparent(position.x, position.y + 1, position.z + 1) || IsFoliage(position.x, position.y + 1, position.z + 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopRight = true;
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    if(position.z == 0 && position.y < World::chunkHeightY - 1) // We are on a z axis chunk border
    {
        if(abs(chunk_position_z - 1) <= World::chunkDiameter) // Check out of chunk
        {
            if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on a x-axis chunk border along with our z-axis chunk border
            {
                if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z - 1)]->IsTransparent(World::chunkWidthX-1, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z - 1)]->IsFoliage(World::chunkWidthX-1, position.y+1, World::chunkDepthZ-1))
                {
                    // We want to do ambient occlusion bottom left and bottom right
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopLeft = true;
                    tempBlock.topFace.aoBottomLeft = true;
                    SetBlock(tempBlock);                                    
                }
            }
            else if(position.x == World::chunkWidthX - 1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on a x-axis chunk border along with our z-axis chunk border
            {
                if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z - 1)]->IsTransparent(0, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z - 1)]->IsFoliage(0, position.y+1, World::chunkDepthZ-1))
                {
                    // We want to do ambient occlusion bottom left and bottom right
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopLeft = true;
                    tempBlock.topFace.aoBottomLeft = true;
                    SetBlock(tempBlock);                                    
                }
            }
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsTransparent(position.x, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsFoliage(position.x, position.y+1, World::chunkDepthZ-1))
            {
                // We want to do ambient occlusion bottom left and bottom right
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopLeft = true;
                tempBlock.topFace.aoBottomLeft = true;
                SetBlock(tempBlock);                                    
            }
        }
        // Check in the same chunk
        if(!IsTransparent(position.x, position.y + 1, position.z + 1) || IsFoliage(position.x, position.y + 1, position.z + 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopRight = true;
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    if(position.z == World::chunkDepthZ - 1 && position.y < World::chunkHeightY - 1)  // We are on a z axis chunk border
    {
        if(abs(chunk_position_z + 1) <= World::chunkDiameter) // Check out of chunk
        {
            if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on a x-axis chunk border along with our z-axis chunk border
            {
                if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z + 1)]->IsTransparent(World::chunkWidthX-1, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z + 1)]->IsFoliage(World::chunkWidthX-1, position.y+1, 0))
                {
                    // We want to do ambient occlusion bottom left and bottom right
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopRight = true;
                    tempBlock.topFace.aoBottomRight = true;
                    SetBlock(tempBlock);                                    
                }
            }
            else if(position.x == World::chunkWidthX - 1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on a x-axis chunk border along with our z-axis chunk border
            {
                if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z + 1)]->IsTransparent(0, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z + 1)]->IsFoliage(0, position.y+1, 0))
                {
                    // We want to do ambient occlusion bottom left and bottom right
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopRight = true;
                    tempBlock.topFace.aoBottomRight = true;
                    SetBlock(tempBlock);                                    
                }
            }
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsTransparent(position.x, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsFoliage(position.x, position.y+1, 0))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopRight = true;
                tempBlock.topFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
        // Check in the same chunk
        if(!IsTransparent(position.x, position.y + 1, position.z - 1) || IsFoliage(position.x, position.y + 1, position.z - 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopLeft = true;
            tempBlock.topFace.aoBottomLeft = true;
            SetBlock(tempBlock);
        }
    }

    // For blocks up and diagonal not on chunk border
    if(position.z != 0 && position.z < World::chunkDepthZ - 1 && position.x != 0 && position.x < World::chunkWidthX - 1 && position.y < World::chunkHeightY - 1)
    {   
        if(!IsTransparent(position.x + 1, position.y + 1, position.z - 1) || IsFoliage(position.x + 1, position.y + 1, position.z - 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopLeft = true;
            SetBlock(tempBlock);
        }
        if(!IsTransparent(position.x + 1, position.y + 1, position.z + 1) || IsFoliage(position.x + 1, position.y + 1, position.z + 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopRight = true;
            SetBlock(tempBlock);
        }
        if(!IsTransparent(position.x - 1, position.y + 1, position.z + 1) || IsFoliage(position.x - 1, position.y + 1, position.z + 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
        if(!IsTransparent(position.x - 1, position.y + 1, position.z - 1) || IsFoliage(position.x - 1, position.y + 1, position.z - 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomLeft = true;
            SetBlock(tempBlock);
        }
    }
    // For blocks up and diagonal on chunk border
    // Z level checking in the negative z direction
    if(position.z == 0 && position.y < World::chunkHeightY - 1)
    {
        if(abs(chunk_position_z - 1) <= World::chunkDiameter) // Check out of chunk
        {
            // I may need an additional check here because I am doing x+1 and x-1 as well.
            // My gut feeling is this will segmentation fault across the x-axis if a block in the exact corner
            // of a chunk needs AO, but will see in the future
            if(position.x == World::chunkWidthX - 1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well
            {
                if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z - 1)]->IsTransparent(0, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z - 1)]->IsFoliage(0, position.y+1, World::chunkDepthZ-1))
                {
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopLeft = true;
                    SetBlock(tempBlock);
                }
            }
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsTransparent(position.x+1, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsFoliage(position.x+1, position.y+1, World::chunkDepthZ-1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopLeft = true;
                SetBlock(tempBlock);
            }
            if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well
            {
                if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z - 1)]->IsTransparent(World::chunkWidthX-1, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z - 1)]->IsFoliage(World::chunkWidthX-1, position.y+1, World::chunkDepthZ-1))
                {
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoBottomLeft = true;
                    SetBlock(tempBlock);
                }
            }
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsTransparent(position.x-1, position.y+1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsFoliage(position.x-1, position.y+1, World::chunkDepthZ-1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoBottomLeft = true;
                SetBlock(tempBlock);
            }
        }
        // Check inside the chunk
        if(position.x == World::chunkWidthX - 1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
        {
            if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsTransparent(0, position.y+1, position.z+1) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsFoliage(0, position.y+1, position.z+1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopRight = true;
                SetBlock(tempBlock);
            }
        }
        else if(!IsTransparent(position.x+1, position.y+1, position.z+1) || IsFoliage(position.x+1, position.y+1, position.z+1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopRight = true;
            SetBlock(tempBlock);
        }
        if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
        {
            if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsTransparent(World::chunkWidthX-1, position.y+1, position.z+1) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsFoliage(World::chunkWidthX-1, position.y+1, position.z+1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
        else if(!IsTransparent(position.x-1, position.y+1, position.z+1) || IsFoliage(position.x-1, position.y+1, position.z+1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    // For blocks up and diagonal on chunk border
    // Z level checking in the positive z direction
    if(position.z == World::chunkDepthZ - 1 && position.y < World::chunkHeightY - 1)
    {
        if(abs(chunk_position_z + 1) <= World::chunkDiameter) // Check out of chunk
        {
            // I may need an additional check here because I am doing x+1 and x-1 as well.
            // My gut feeling is this will segmentation fault across the x-axis if a block in the exact corner
            // of a chunk needs AO, but will see in the future
            if(position.x == World::chunkWidthX-1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
            {
                if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z + 1)]->IsTransparent(0, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z + 1)]->IsFoliage(0, position.y+1, 0))
                {
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoTopRight = true;
                    SetBlock(tempBlock);
                }
            }
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsTransparent(position.x+1, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsFoliage(position.x+1, position.y+1, 0))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopRight = true;
                SetBlock(tempBlock);
            }
            if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
            {
                if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z + 1)]->IsTransparent(World::chunkWidthX-1, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z + 1)]->IsFoliage(World::chunkWidthX-1, position.y+1, 0))
                {
                    Block tempBlock = GetBlock(position);
                    tempBlock.topFace.aoBottomRight = true;
                    SetBlock(tempBlock);
                }
            }
            else if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsTransparent(position.x-1, position.y+1, 0) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsFoliage(position.x-1, position.y+1, 0))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
        // Check inside the chunk
        if(position.x == World::chunkWidthX-1 && abs(chunk_position_x + 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
        {
            if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsTransparent(0, position.y+1, position.x-1) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsFoliage(0, position.y+1, position.z-1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoTopLeft = true;
                SetBlock(tempBlock);
            }
        }
        else if(!IsTransparent(position.x + 1, position.y + 1, position.z - 1) || IsFoliage(position.x + 1, position.y + 1, position.z - 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoTopLeft = true;
            SetBlock(tempBlock);
        }
        if(position.x == 0 && abs(chunk_position_x - 1) <= World::chunkDiameter) // If we are on an x-axis chunk border as well 
        {
            if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsTransparent(World::chunkWidthX-1, position.y+1, position.z-1) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsFoliage(World::chunkWidthX-1, position.y+1, position.z-1))
            {
                Block tempBlock = GetBlock(position);
                tempBlock.topFace.aoBottomLeft = true;
                SetBlock(tempBlock);
            }
        }
        else if(!IsTransparent(position.x - 1, position.y + 1, position.z - 1) || IsFoliage(position.x - 1, position.y + 1, position.z - 1))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.topFace.aoBottomLeft = true;
            SetBlock(tempBlock);
        }
    }
    // For blocks up and diagonal on chunk border
    // X level checking in the negative X direction

    // For blocks up and diagonal on chunk border
    // X level checking in the positive X direction
}



void Chunk::DetermineAOFrontFace(glm::vec3 position)
{
    // If we are not on a z-axis chunk border
    if(position.z < World::chunkDepthZ - 1 && position.y != 0)
    {
        if((!IsTransparent(position.x, position.y - 1, position.z + 1) || IsFoliage(position.x, position.y - 1, position.z + 1)))
        {
            Block tempBlock = GetBlock(position);
            tempBlock.frontFace.aoBottomLeft = true;
            tempBlock.frontFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    else if(position.z == World::chunkDepthZ - 1 && position.y != 0) // We are on a z-axis chunk border
    {
        if(abs(chunk_position_z + 1) <= World::chunkDiameter) 
        {
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsTransparent(position.x, position.y-1, 0) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsFoliage(position.x, position.y-1, 0))
            {
                // We want to do ambient occlusion top left and bottom left
                Block tempBlock = GetBlock(position);
                tempBlock.frontFace.aoBottomLeft = true;
                tempBlock.frontFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
    } 
}



void Chunk::DetermineAOBackFace(glm::vec3 position)
{
    // If we are not on a z-axis chunk border
    if(position.z != 0 && position.y != 0)
    {
        if((!IsTransparent(position.x, position.y - 1, position.z - 1) || IsFoliage(position.x, position.y - 1, position.z - 1)))
        {
            // We want to do ambient occlusion top left and bottom left
            Block tempBlock = GetBlock(position);
            tempBlock.backFace.aoBottomLeft = true;
            tempBlock.backFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    else if(position.z == 0 && position.y != 0) // We are on a z-axis chunk border
    {
        if(abs(chunk_position_z - 1) <= World::chunkDiameter)
        {
            if(!chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsTransparent(position.x, position.y-1, World::chunkDepthZ-1) || chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsFoliage(position.x, position.y-1, World::chunkDepthZ-1))
            {                                        
                // We want to do ambient occlusion top left and bottom left
                Block tempBlock = GetBlock(position);
                tempBlock.backFace.aoBottomLeft = true;
                tempBlock.backFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
    } 
}



void Chunk::DetermineAOLeftFace(glm::vec3 position)
{
    // If we are not on an x-axis chunk border
    if(position.x != 0 && position.y != 0)
    {
        // Check if there is a block over and up 1 to the left
        if((!IsTransparent(position.x - 1, position.y - 1, position.z) || IsFoliage(position.x - 1, position.y - 1, position.z)))
        {
            // We want to do ambient occlusion bottom left and bottom right   
            Block tempBlock = GetBlock(position);
            tempBlock.leftFace.aoBottomLeft = true;
            tempBlock.leftFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    else if(position.x == 0 && position.y != 0) // We are on a x-axis chunk border
    {
        if(abs(chunk_position_x - 1) <= World::chunkDiameter)
        {
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            if(!chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsTransparent(World::chunkWidthX-1, position.y-1, position.z) || chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsFoliage(World::chunkWidthX-1, position.y-1, position.z))
            {
                // We want to do ambient occlusion bottom left and bottom right
                Block tempBlock = GetBlock(position);
                tempBlock.leftFace.aoBottomLeft = true;
                tempBlock.leftFace.aoBottomRight = true;
                SetBlock(tempBlock);                                    
            }
        }
    } 
}



void Chunk::DetermineAORightFace(glm::vec3 position)
{
    // If we are not on an x-axis chunk border
    if(position.x < World::chunkWidthX - 1 && position.y != 0)
    {
        if((!IsTransparent(position.x + 1, position.y - 1, position.z) || IsFoliage(position.x + 1, position.y - 1, position.z)))
        {
            // We want to do ambient occlusion top left and bottom left
            Block tempBlock = GetBlock(position);
            tempBlock.rightFace.aoBottomLeft = true;
            tempBlock.rightFace.aoBottomRight = true;
            SetBlock(tempBlock);
        }
    }
    else if(position.x == World::chunkWidthX - 1 && position.y != 0) // We are on a x-axis chunk border
    {
        if(abs(chunk_position_x + 1) <= World::chunkDiameter) 
        {
            // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
            if(!chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsTransparent(0, position.y-1, position.z) || chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsFoliage(0, position.y-1, position.z))
            {
                // We want to do ambient occlusion bottom left and bottom right
                Block tempBlock = GetBlock(position);
                tempBlock.rightFace.aoBottomLeft = true;
                tempBlock.rightFace.aoBottomRight = true;
                SetBlock(tempBlock);
            }
        }
    } 
}



// Draw an individual face
void Chunk::DrawFace(Block block, GLuint faceIndex)
{
    GLuint posX = block.position.x;     // 6 Bits, 0-63
    GLuint posY = block.position.y;     // 6 Bits, 0-63
    GLuint posZ = block.position.z;     // 6 Bits, 0-63
    GLuint faceID = faceIndex;          // 3 Bits, 0-7
    // VertexID                         // 3 Bits, 0-7
    GLuint textureID = block.blockTypeID; // 5 Bits, 0-31
    // Ambient Occlusion Enabled        // 1 Bit,  0-1

    GLuint packedVertexOne;
    GLuint packedVertexTwo;
    GLuint packedVertexThree;
    GLuint packedVertexFour;
    GLuint packedVertexFive;
    GLuint packedVertexSix;
    
    // Pack our data up
    switch(faceIndex) // Ambient Occlusion data is different per vertex depending on the face, hence a switch
    {
        case BlockFaces::Top_Face:
            if(World::ambientOcclusionEnabled)
            {
                // Reset ambient occlusion fields in case we are breaking a block and need
                // to recalculate them          
                block.topFace.aoTopLeft = false;
                block.topFace.aoTopRight = false;
                block.topFace.aoBottomLeft = false;
                block.topFace.aoBottomRight = false;
                SetBlock(block);
                // Determine ambient occlusion for this block
                DetermineAOTopFace(block.position);
                block = GetBlock(block.position);
            }
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | block.topFace.aoBottomLeft << 29); 
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | block.topFace.aoTopRight << 29);
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | block.topFace.aoTopLeft << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | block.topFace.aoTopRight << 29);
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | block.topFace.aoBottomLeft << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | block.topFace.aoBottomRight << 29);
            break;
        case BlockFaces::Front_Face:
            if(World::ambientOcclusionEnabled)
            {
                // Reset ambient occlusion fields in case we are breaking a block and need
                // to recalculate them          
                block.frontFace.aoTopLeft = false;
                block.frontFace.aoTopRight = false;
                block.frontFace.aoBottomLeft = false;
                block.frontFace.aoBottomRight = false;
                SetBlock(block);
                // Determine ambient occlusion for this block
                DetermineAOFrontFace(block.position);
                block = GetBlock(block.position);
            }
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | block.frontFace.aoBottomLeft << 29); 
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | 0 << 29);
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | 0 << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | 0 << 29);
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | block.frontFace.aoBottomLeft << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | block.frontFace.aoBottomRight << 29);
            break;
        case BlockFaces::Back_Face:
            if(World::ambientOcclusionEnabled)
            {
                // Reset ambient occlusion fields in case we are breaking a block and need
                // to recalculate them          
                block.backFace.aoTopLeft = false;
                block.backFace.aoTopRight = false;
                block.backFace.aoBottomLeft = false;
                block.backFace.aoBottomRight = false;
                SetBlock(block);
                // Determine ambient occlusion for this block
                DetermineAOBackFace(block.position);
                block = GetBlock(block.position);
            }
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | block.backFace.aoBottomRight << 29); 
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | 0 << 29);
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | block.backFace.aoBottomLeft << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | 0 << 29);
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | block.backFace.aoBottomRight << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | 0 << 29);
            break;
        case BlockFaces::Left_Face:
            if(World::ambientOcclusionEnabled)
            {
                // Reset ambient occlusion fields in case we are breaking a block and need
                // to recalculate them          
                block.leftFace.aoTopLeft = false;
                block.leftFace.aoTopRight = false;
                block.leftFace.aoBottomLeft = false;
                block.leftFace.aoBottomRight = false;
                SetBlock(block);
                // Determine ambient occlusion for this block
                DetermineAOLeftFace(block.position);
                block = GetBlock(block.position);
            }
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | 0 << 29); 
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | block.leftFace.aoBottomLeft << 29); 
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | block.leftFace.aoBottomRight << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | block.leftFace.aoBottomLeft << 29); 
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | 0 << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | 0 << 29); 
            break;
        case BlockFaces::Right_Face:
            if(World::ambientOcclusionEnabled)
            {
                // Reset ambient occlusion fields in case we are breaking a block and need
                // to recalculate them          
                block.rightFace.aoTopLeft = false;
                block.rightFace.aoTopRight = false;
                block.rightFace.aoBottomLeft = false;
                block.rightFace.aoBottomRight = false;
                SetBlock(block);
                // Determine ambient occlusion for this block
                DetermineAORightFace(block.position);
                block = GetBlock(block.position);
            }
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | 0 << 29); 
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | block.rightFace.aoBottomRight << 29);
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | 0 << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | block.rightFace.aoBottomRight << 29);
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | 0 << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | block.rightFace.aoBottomLeft << 29);
            break;
        default:
            packedVertexOne   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 0 << 21 | textureID << 24 | 0 << 29);
            packedVertexTwo   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 1 << 21 | textureID << 24 | 0 << 29);
            packedVertexThree = (posX | posY << 6 | posZ << 12 | faceID << 18 | 2 << 21 | textureID << 24 | 0 << 29); 
            packedVertexFour  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 3 << 21 | textureID << 24 | 0 << 29);
            packedVertexFive  = (posX | posY << 6 | posZ << 12 | faceID << 18 | 4 << 21 | textureID << 24 | 0 << 29); 
            packedVertexSix   = (posX | posY << 6 | posZ << 12 | faceID << 18 | 5 << 21 | textureID << 24 | 0 << 29);
            break;
    }

    if(IsTransparent(block.position)) // Our block is transparent
    { 
        chunkTransparentVertices.push_back(packedVertexOne);
        chunkTransparentVertices.push_back(packedVertexTwo);
        chunkTransparentVertices.push_back(packedVertexThree);
        chunkTransparentVertices.push_back(packedVertexFour);
        chunkTransparentVertices.push_back(packedVertexFive);
        chunkTransparentVertices.push_back(packedVertexSix); 
    }
    else // Our block is not transparent
    {
        chunkOpaqueVertices.push_back(packedVertexOne);
        chunkOpaqueVertices.push_back(packedVertexTwo);
        chunkOpaqueVertices.push_back(packedVertexThree);
        chunkOpaqueVertices.push_back(packedVertexFour);
        chunkOpaqueVertices.push_back(packedVertexFive);
        chunkOpaqueVertices.push_back(packedVertexSix);
    }
}



void Chunk::RenderMesh()
{
    for(GLfloat z = 0; z < World::chunkDepthZ;  z += 1)
    for(GLfloat x = 0; x < World::chunkWidthX;  x += 1)
    for(GLfloat y = 0; y < World::chunkHeightY; y += 1)            
    { 
        // If this isn't an air block
        if(GetBlock(x, y, z).blockTypeID != blocks["Air"]["index"])
        {
            // The block we are currently testing to see if we render its faces
            Block block = GetBlock(x, y, z); 

            // If any given face for a block has an adjacent air block,
            // we will want to render that block. These if statements 
            // do all of that adjacent air block checking. The else conditions
            // are for checking blocks that are on chunk borders

            /* Y axis checking for neighbouring air blocks */
            if(y < World::chunkHeightY - 1)
            {
                // std::cout << "Check: " << IsTransparent(x, y+1, z) << std::endl;
                if(IsTransparent(x, y+1, z)) // This is a surface block
                {
                    // Draw Top Face of this block
                    DrawFace(block, BlockFaces::Top_Face);
                }
            }
            else // if y == World::chunkHeightY - 1, on chunk border
            {
                if(chunk_position_y + 1 < World::chunksTall) 
                {
                    // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
                    if(chunks_[glm::vec3(chunk_position_x, chunk_position_y + 1, chunk_position_z)]->IsTransparent(x, 0, z)) 
                    {
                        // Draw Top Face of this block
                        DrawFace(block, BlockFaces::Top_Face);
                    }
                }
            }
            
            // Checking for y level
            if(y != 0)
            {
                if(IsTransparent(x, y-1, z))
                {
                    // Draw Bottom Face of this block
                    DrawFace(block, BlockFaces::Bottom_Face);
                }
            }
            else // if y == 0, on chunk border
            {
                if(chunk_position_y - 1 >= 0)
                {
                    // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
                    if(chunks_[glm::vec3(chunk_position_x, chunk_position_y - 1, chunk_position_z)]->IsTransparent(x, World::chunkHeightY - 1, z))
                    {                                    
                        // Draw Bottom Face of this block
                        DrawFace(block, BlockFaces::Bottom_Face);
                    }
                }
            }

            // Only do x and z faces if this block is not transparent unless its foliage
            if(!IsTransparent(x, y, z) || IsFoliage(x, y, z))
            {
                // X axis checking for neighbouring air blocks //   
                // If its != 0, its not on a chunk border
                if(x != 0)
                {
                    if(IsTransparent(x-1, y, z))
                    {
                        // Draw Left Face of this block
                        DrawFace(block, BlockFaces::Left_Face);
                    }
                }
                else // if x == 0, on chunk border
                {
                    if(abs(chunk_position_x - 1) <= World::chunkDiameter)
                    {
                        // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
                        if(chunks_[glm::vec3(chunk_position_x - 1, chunk_position_y, chunk_position_z)]->IsTransparent(World::chunkWidthX-1, y, z))
                        {                                    
                            // Draw Left Face of this block
                            DrawFace(block, BlockFaces::Left_Face);
                        }
                    }
                }

                // If its not at the chunk's max position, its not on a chunk border
                if(x < World::chunkWidthX - 1)
                {
                    if(IsTransparent(x+1, y, z))
                    {
                        // Draw Right Face of this block
                        DrawFace(block, BlockFaces::Right_Face);
                    }
                }
                else // if x == World::chunkWidthX - 1, on chunk border
                {
                    if(abs(chunk_position_x + 1) <= World::chunkDiameter) 
                    {
                        // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
                        if(chunks_[glm::vec3(chunk_position_x + 1, chunk_position_y, chunk_position_z)]->IsTransparent(0, y, z))
                        {
                            // Draw Right Face of this block
                            DrawFace(block, BlockFaces::Right_Face);
                        }
                    }
                }

                // Z axis checking for neighbouring air blocks //
                // If its != 0, its not on a chunk border
                if(z != 0)
                {
                    if(IsTransparent(x, y, z-1))
                    {
                        // Draw Back Face of this block
                        DrawFace(block, BlockFaces::Back_Face);
                    }
                }
                else // if z == 0, on chunk border
                {
                    if(abs(chunk_position_z - 1) <= World::chunkDiameter)
                    {
                        if(chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z - 1)]->IsTransparent(x, y, World::chunkDepthZ-1))
                        {                                        
                            // Draw Back Face of this block
                            DrawFace(block, BlockFaces::Back_Face);
                        }
                    }
                }
                
                // If its not at the chunk's max position, its not on a chunk border
                if(z < World::chunkDepthZ - 1)
                {
                    if(IsTransparent(x, y, z+1))
                    {
                        // Draw Front Face of this block
                        DrawFace(block, BlockFaces::Front_Face);
                    }
                }
                else // if z == World::chunkDepthZ - 1, on chunk border
                {
                    if(abs(chunk_position_z + 1) <= World::chunkDiameter) 
                    {
                        // X Position of adjacent chunk, y Position, Z position, position of chunk, chunks pointer
                        if(chunks_[glm::vec3(chunk_position_x, chunk_position_y, chunk_position_z + 1)]->IsTransparent(x, y, 0))
                        {
                            // Draw Front Face of this block
                            DrawFace(block, BlockFaces::Front_Face);
                        }
                    }
                }
            } 
        }
    }
}



void Chunk::RebuildMesh()
{
    // Clear our batch data
    chunkOpaqueVertices.clear();
    chunkTransparentVertices.clear();

    // Remesh our chunk
    meshCreated = false;
}



void Chunk::RenderChunk(GLuint cubeShaderProgramID, GLboolean renderOpaque)
{
    // Draw our chunks, this if statement only runs once per chunk
    if(!meshCreated)
    {
        /* Create our batch data */
        // We have to create our mesh for our chunk using our 3D chunk block array
        meshCreated = true;
        RenderMesh();

        // Put our batch data in buffers
        // Bind the VAO so OpenGL knows to use it
        // Batch data for Opaque blocks
        ChunkOpaqueVAO.Bind();
        ChunkOpaqueVBO.InitVBO(chunkOpaqueVertices.data(), sizeof(GLuint) * chunkOpaqueVertices.size());
        ChunkOpaqueVAO.Unbind();

        // Batch data for Transparent blocks
        ChunkTransparentVAO.Bind();
        ChunkTransparentVBO.InitVBO(chunkTransparentVertices.data(), sizeof(GLuint) * chunkTransparentVertices.size());
        ChunkTransparentVAO.Unbind();
    }

    // Set our uniform for our chunk offset
    glUniform3f(glGetUniformLocation(cubeShaderProgramID, "chunkOffset"), offset_x, offset_y, offset_z);

    // Are we rendering opaque or transparent faces
    if(renderOpaque)
    {
        ChunkOpaqueVAO.Bind();
        // Render our opaque faces
        glDrawArrays(GL_TRIANGLES, 0, chunkOpaqueVertices.size());
        ChunkOpaqueVAO.Unbind();
    }
    else
    {
        ChunkTransparentVAO.Bind();
        // Render our transparent faces
        glDrawArrays(GL_TRIANGLES, 0, chunkTransparentVertices.size());
        ChunkTransparentVAO.Unbind();
    }
}



// Utility method for 1D chunk array. Set a block in the array
void Chunk::SetBlock(Block block)
{
    chunk[(GLint)block.position.x + (GLint)block.position.z*World::chunkWidthX + (GLint)block.position.y*World::chunkWidthX*World::chunkDepthZ] = block;
}



// Set blockType for block
void Chunk::SetBlockType(glm::vec3 position, GLint BlockTypeID)
{
    chunk[(GLint)position.x + (GLint)position.z*World::chunkWidthX + (GLint)position.y*World::chunkWidthX*World::chunkDepthZ].blockTypeID = BlockTypeID; 
}



// Utility method for 1D chunk array. Get a block in the array
Block Chunk::GetBlock(GLint x, GLint y, GLint z)
{
    return chunk[x + z*World::chunkWidthX + y*World::chunkWidthX*World::chunkDepthZ];
}



// Utility method for 1D chunk array. Get a block in the array
Block Chunk::GetBlock(glm::vec3 position)
{
    return chunk[(GLint)position.x + (GLint)position.z*World::chunkWidthX + (GLint)position.y*World::chunkWidthX*World::chunkDepthZ];
}



// Utility method for 1D chunk array. Set the position for a block in the array
void Chunk::SetPosition(GLint x, GLint y, GLint z)
{
    chunk[x + z*World::chunkWidthX + y*World::chunkWidthX*World::chunkDepthZ].position = glm::vec3(x, y, z);
}



GLboolean Chunk::IsTransparent(GLint x, GLint y, GLint z)
{
    return blocks[std::to_string(chunk[x + z*World::chunkWidthX + y*World::chunkWidthX*World::chunkDepthZ].blockTypeID)]["transparent"];
}



GLboolean Chunk::IsTransparent(glm::vec3 position)
{
    return blocks[std::to_string(chunk[(GLint)position.x + (GLint)position.z*World::chunkWidthX + (GLint)position.y*World::chunkWidthX*World::chunkDepthZ].blockTypeID)]["transparent"];
}



GLboolean Chunk::IsFoliage(GLint x, GLint y, GLint z)
{
    return blocks[std::to_string(chunk[x + z*World::chunkWidthX + y*World::chunkWidthX*World::chunkDepthZ].blockTypeID)]["isFoliage"];
}



GLboolean Chunk::IsFoliage(glm::vec3 position)
{
    return blocks[std::to_string(chunk[(GLint)position.x + (GLint)position.z*World::chunkWidthX + (GLint)position.y*World::chunkWidthX*World::chunkDepthZ].blockTypeID)]["isFoliage"];
}
