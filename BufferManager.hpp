#pragma once

#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "ShaderManager.hpp"
#include "ChunkManager.hpp"
#include "TextureArray.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>



class BufferManager
{
public:
    Shader cubeShaderProgram{"shaders/cube.vert", "shaders/cube.frag"};

    VAO LightVAO;
    VBO LightVBO;
    EBO LightEBO;
    Shader lightShaderProgram{"shaders/lighting.vert", "shaders/lighting.frag"};

    // Our textures
    TextureArray textureArray;

    // Our chunk manager
    ChunkManager chunkManager;

    // Singleton Design
    static BufferManager &Instance()
    {
        static BufferManager instance;
        return instance;
    }
    // Constructor that generates a Vertex Array Object
    BufferManager();
    // Destructor
    ~BufferManager();

    // Run the event loop once
    void RunLoop(GLFWwindow *window, glm::vec3 cameraPosition, glm::vec3 cameraOrientation);
    // Activate textures
    void ActivateTextures();
};