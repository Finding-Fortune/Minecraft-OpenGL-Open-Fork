#include "Player.hpp"
#include "WindowManager.hpp"
#include "WorldConstants.hpp"
#include "Chunk.hpp"
#include "Block.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <iostream>
#include <math.h>  



Player::Player()
{
    WindowManager &windowManager = WindowManager::Instance();
	window_ = windowManager.GetWindow();
}



void Player::ProcessInput(glm::vec3 playerPosition, glm::vec3 playerOrientation)
{
    // Get elapsed time so we can cap input at x times per second
    static GLdouble previous_seconds = glfwGetTime();
    GLdouble current_seconds = glfwGetTime();
    GLdouble elapsed_seconds = current_seconds - previous_seconds;

    // If you press escape on our window, close the window
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
    }
    // Press 1 for wireframe
    if (glfwGetKey(window_, GLFW_KEY_1) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    // Press 2 for filled
    if (glfwGetKey(window_, GLFW_KEY_2) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    // Press f for fullscreen
    if (glfwGetKey(window_, GLFW_KEY_F) == GLFW_PRESS)
    {
        glfwMaximizeWindow(window_);
        glfwPollEvents();
    }

    // Left click for block break
    if(glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (elapsed_seconds > World::blockBreakingSpeed)
        {
            // Reset our timer for counting elapsed time
            previous_seconds = current_seconds;

            // Determine which chunk we are in
            glm::vec3 playerCurrentChunk = glm::vec3(floor((GLfloat)playerPosition.x / (GLfloat)World::chunkWidthX), floor((GLfloat)playerPosition.y / (GLfloat)World::chunkHeightY), floor((GLfloat)playerPosition.z / (GLfloat)World::chunkDepthZ));

            if(chunks_.find(playerCurrentChunk) != chunks_.end())
            {
                // Get our chunk offsets so we can determine the local position of blocks
                // in their chunk given their world/global position
                GLint chunk_offset_x = chunks_[playerCurrentChunk]->offset_x;
                GLint chunk_offset_y = chunks_[playerCurrentChunk]->offset_y;
                GLint chunk_offset_z = chunks_[playerCurrentChunk]->offset_z;

                GLfloat stepScaleAmount = 0.1f ;
                GLuint index = 0;
                // Step forward 100 times towards the player's orientation until we hit a limit
                // or we hit a block we can break
                glm::vec3 tempPlayerPosition = glm::vec3((GLint)playerPosition.x - chunk_offset_x + (playerPosition.x < 0 ? -1 : 0), (GLint)playerPosition.y - chunk_offset_y, (GLint)playerPosition.z - chunk_offset_z + (playerPosition.z < 0 ? -1 : 0));
                while(chunks_[playerCurrentChunk]->GetBlock(tempPlayerPosition).blockTypeID == blocks["Air"]["index"] && index < World::playerReachScaleAmount)
                {
                    playerPosition = glm::vec3(playerPosition.x + playerOrientation.x * stepScaleAmount , playerPosition.y + playerOrientation.y * stepScaleAmount , playerPosition.z + playerOrientation.z * stepScaleAmount );
                    playerCurrentChunk = glm::vec3(floor(playerPosition.x / ((GLfloat)World::chunkWidthX )), floor(playerPosition.y / ((GLfloat)World::chunkHeightY )), floor(playerPosition.z / ((GLfloat)World::chunkDepthZ )));
                    // If the new block position we step forward to is in a chunk that is not rendered, break out of the loop
                    if(playerPosition.y < 0 || chunks_.find(playerCurrentChunk) == chunks_.end())// We went out of bounds
                    {
                        break;
                    }
                    chunk_offset_x = chunks_[playerCurrentChunk]->offset_x;
                    chunk_offset_y = chunks_[playerCurrentChunk]->offset_y;
                    chunk_offset_z = chunks_[playerCurrentChunk]->offset_z;
                    tempPlayerPosition = glm::vec3((GLint)playerPosition.x - chunk_offset_x + (playerPosition.x < 0 ? -1 : 0), (GLint)playerPosition.y - chunk_offset_y, (GLint)playerPosition.z - chunk_offset_z + (playerPosition.z < 0 ? -1 : 0));
                    index++;
                }
                // If we did not go out of bounds
                if (chunks_.find(playerCurrentChunk) != chunks_.end() && chunks_[playerCurrentChunk]->GetBlock(tempPlayerPosition).blockTypeID != blocks["Air"]["index"])
                {
                    // Set the block we are in to air
                    chunks_[playerCurrentChunk]->SetBlockType(tempPlayerPosition, blocks["Air"]["index"]);
                    // Rebuild mesh
                    chunks_[playerCurrentChunk]->RebuildMesh();
                    // If this block is on a chunk border, also
                    // update that bordering's chunks mesh
                    if (tempPlayerPosition.x == 0 && chunks_.find(glm::vec3(playerCurrentChunk.x - 1, playerCurrentChunk.y, playerCurrentChunk.z)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x - 1, playerCurrentChunk.y, playerCurrentChunk.z)]->RebuildMesh();
                    }
                    else if (tempPlayerPosition.x == World::chunkWidthX - 1 && chunks_.find(glm::vec3(playerCurrentChunk.x + 1, playerCurrentChunk.y, playerCurrentChunk.z)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x + 1, playerCurrentChunk.y, playerCurrentChunk.z)]->RebuildMesh();
                    }

                    // For now we are not updating y level chunk borders

                    if (tempPlayerPosition.z == 0 && chunks_.find(glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z - 1)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z - 1)]->RebuildMesh();
                    }
                    else if (tempPlayerPosition.z == World::chunkDepthZ - 1 && chunks_.find(glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z + 1)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z + 1)]->RebuildMesh();
                    }
                }
            }
        }
    }


    // Right click for block place
    if(glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (elapsed_seconds > World::blockPlacingSpeed)
        {
            // Reset our timer for counting elapsed time
            previous_seconds = current_seconds;

            // Determine which chunk we are in
            glm::vec3 playerCurrentChunk = glm::vec3(floor((GLfloat)playerPosition.x / (GLfloat)World::chunkWidthX), floor((GLfloat)playerPosition.y / (GLfloat)World::chunkHeightY), floor((GLfloat)playerPosition.z / (GLfloat)World::chunkDepthZ));

            if(chunks_.find(playerCurrentChunk) != chunks_.end())
            {
                // Get our chunk offsets so we can determine the local position of blocks
                // in their chunk given their world/global position
                GLint chunk_offset_x = chunks_[playerCurrentChunk]->offset_x;
                GLint chunk_offset_y = chunks_[playerCurrentChunk]->offset_y;
                GLint chunk_offset_z = chunks_[playerCurrentChunk]->offset_z;

                GLfloat stepScaleAmount = 0.1f ;
                GLuint index = 0;
                // Step forward 100 times towards the player's orientation until we hit a limit
                // or we hit a block we can break
                glm::vec3 tempPlayerPosition = glm::vec3((GLint)playerPosition.x - chunk_offset_x + (playerPosition.x < 0 ? -1 : 0), (GLint)playerPosition.y - chunk_offset_y, (GLint)playerPosition.z - chunk_offset_z + (playerPosition.z < 0 ? -1 : 0));
                while(chunks_[playerCurrentChunk]->GetBlock(tempPlayerPosition).blockTypeID == blocks["Air"]["index"] && index < World::playerReachScaleAmount)
                {
                    playerPosition = glm::vec3(playerPosition.x + playerOrientation.x * stepScaleAmount , playerPosition.y + playerOrientation.y * stepScaleAmount , playerPosition.z + playerOrientation.z * stepScaleAmount );
                    playerCurrentChunk = glm::vec3(floor(playerPosition.x / ((GLfloat)World::chunkWidthX )), floor(playerPosition.y / ((GLfloat)World::chunkHeightY )), floor(playerPosition.z / ((GLfloat)World::chunkDepthZ )));
                    // If the new block position we step forward to is in a chunk that is not rendered, break out of the loop
                    if(playerPosition.y < 0 || chunks_.find(playerCurrentChunk) == chunks_.end())// We went out of bounds
                    {
                        break;
                    }
                    chunk_offset_x = chunks_[playerCurrentChunk]->offset_x;
                    chunk_offset_y = chunks_[playerCurrentChunk]->offset_y;
                    chunk_offset_z = chunks_[playerCurrentChunk]->offset_z;
                    tempPlayerPosition = glm::vec3((GLint)playerPosition.x - chunk_offset_x + (playerPosition.x < 0 ? -1 : 0), (GLint)playerPosition.y - chunk_offset_y, (GLint)playerPosition.z - chunk_offset_z + (playerPosition.z < 0 ? -1 : 0));
                    index++;
                }
                // Go back one spot so we are back to air
                playerPosition = glm::vec3(playerPosition.x - playerOrientation.x * stepScaleAmount , playerPosition.y - playerOrientation.y * stepScaleAmount , playerPosition.z - playerOrientation.z * stepScaleAmount );
                playerCurrentChunk = glm::vec3(floor(playerPosition.x / ((GLfloat)World::chunkWidthX )), floor(playerPosition.y / ((GLfloat)World::chunkHeightY )), floor(playerPosition.z / ((GLfloat)World::chunkDepthZ )));
                chunk_offset_x = chunks_[playerCurrentChunk]->offset_x;
                chunk_offset_y = chunks_[playerCurrentChunk]->offset_y;
                chunk_offset_z = chunks_[playerCurrentChunk]->offset_z;
                tempPlayerPosition = glm::vec3((GLint)playerPosition.x - chunk_offset_x + (playerPosition.x < 0 ? -1 : 0), (GLint)playerPosition.y - chunk_offset_y, (GLint)playerPosition.z - chunk_offset_z + (playerPosition.z < 0 ? -1 : 0));
                // If we did not go out of bounds and the block is air, we can place a block
                if (chunks_.find(playerCurrentChunk) != chunks_.end() && chunks_[playerCurrentChunk]->GetBlock(tempPlayerPosition).blockTypeID == blocks["Air"]["index"])
                {
                    // Set the block we are in
                    chunks_[playerCurrentChunk]->SetBlockType(tempPlayerPosition, blocks["Grass_Top"]["index"]);
                    // Rebuild mesh
                    chunks_[playerCurrentChunk]->RebuildMesh();
                    // If this block is on a chunk border, also
                    // update that bordering's chunks mesh
                    if (tempPlayerPosition.x == 0 && chunks_.find(glm::vec3(playerCurrentChunk.x - 1, playerCurrentChunk.y, playerCurrentChunk.z)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x - 1, playerCurrentChunk.y, playerCurrentChunk.z)]->RebuildMesh();
                    }
                    else if (tempPlayerPosition.x == World::chunkWidthX - 1 && chunks_.find(glm::vec3(playerCurrentChunk.x + 1, playerCurrentChunk.y, playerCurrentChunk.z)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x + 1, playerCurrentChunk.y, playerCurrentChunk.z)]->RebuildMesh();
                    }

                    // For now we are not updating y level chunk borders

                    if (tempPlayerPosition.z == 0 && chunks_.find(glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z - 1)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z - 1)]->RebuildMesh();
                    }
                    else if (tempPlayerPosition.z == World::chunkDepthZ - 1 && chunks_.find(glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z + 1)) != chunks_.end())
                    {
                        chunks_[glm::vec3(playerCurrentChunk.x, playerCurrentChunk.y, playerCurrentChunk.z + 1)]->RebuildMesh();
                    }
                }
            }
        }
    }
}


