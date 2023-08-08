#include "BufferManager.hpp"
#include "resources/Models/Light.cpp"

#define STB_IMAGE_IMPLEMENTATION // Have to include this or STB will throw error
#include <stb/stb_image.h>       // Used for textures / image processing
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Initialize all of our buffers
BufferManager::BufferManager() : cubeShaderProgram("shaders/cube.vert", "shaders/cube.frag") // Member-Initializer List
{
    LightVAO.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    LightVBO.InitVBO(lightVertices, sizeof(lightVertices));
    // Generates Element Buffer Object and links it to indices
    LightEBO.InitEBO(lightIndices, sizeof(lightIndices));

    // Links VBO attributes such as coordinates and colors to VAO
    LightVAO.LinkAttrib(LightVBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (void *)0);

    // Unbind all to prevent accidentally modifying them
    LightVAO.Unbind();
    LightVBO.Unbind();
    LightEBO.Unbind();

    // Activate all of our textures
    ActivateTextures();

    // Generate our chunks if they have not been generated yet
    chunkManager.GenerateChunks();

    // Set our blocksize in our vertex shader
    glUniform1f(glGetUniformLocation(cubeShaderProgram.GetID(), "blockSize"), World::blockSize);
}

BufferManager::~BufferManager()
{
    LightVAO.Delete();
    LightVBO.Delete();
    LightEBO.Delete();
}

void BufferManager::RunLoop(GLFWwindow *window, glm::vec3 cameraPosition, glm::vec3 cameraOrientation)
{
    // Set all of the uniforms for our lighting shader
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);                                    // Color of the light
                                                                                                 // glm::vec3 lightPosition = glm::vec3(sin(glfwGetTime()) * World::chunkSize, World::heightLimit, sin(glfwGetTime()) * World::chunkSize); // Position of the light cube
    glm::vec3 lightPosition = glm::vec3(World::chunkSize, World::heightLimit, World::chunkSize); // Position of the light cube
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    // Tells OpenGL which Shader Program we want to use
    lightShaderProgram.Activate();
    // Export our various uniform values to our lighting shaders
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram.GetID(), "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShaderProgram.GetID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    // Bind the VAO so OpenGL knows to use it
    LightVAO.Bind();
    // Draw primitive, number of indices, datatype of indices, index of indices
    // Draw our light cube
    glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(GLint), GL_UNSIGNED_INT, 0);

    // Tell OpenGL which Shader Program we want to use
    cubeShaderProgram.Activate();
    // Assign uniforms for our cube shader
    glUniform4f(glGetUniformLocation(cubeShaderProgram.GetID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(cubeShaderProgram.GetID(), "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(cubeShaderProgram.GetID(), "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

    // Render all of our chunks
    chunkManager.RenderChunks(cameraPosition, cameraOrientation, cubeShaderProgram.GetID());
}

void BufferManager::ActivateTextures()
{
    // Tell OpenGL which Shader Program we want to use
    cubeShaderProgram.Activate();

    // Load all of our textures from a json file
    std::ifstream ifs("resources/blocks.json");
    json jf;

    if (ifs.is_open())
    {
        jf = json::parse(ifs);
    }
    else
    {
        std::cout << "The texture file could not be opened." << std::endl;
    }

    std::string texture_path;
    std::string group;
    bool texture_transparent;
    bool texture_isFoliage;
    GLuint index = 0;
    GLuint count = 0;

    // Add air tile
    blocks["Air"]["index"] = -1;
    blocks["Air"]["transparent"] = true;
    blocks["Air"]["isFoliage"] = false;
    blocks["Air"]["group"] = "Air";
    blocks["-1"]["transparent"] = true;
    blocks["-1"]["isFoliage"] = false;
    blocks["-1"]["group"] = "Air";

    // Loop through all of our textures. Double for loop
    // makes sure we get all the indices in order
    for (auto it = jf.begin(); it != jf.end(); it++)
    {
        for (auto el : jf.items())
        {
            index = el.value()["id"];
            if (index == count)
            {
                // Get texture path and group from json
                texture_path = el.value()["texture"];
                group = el.value()["group"];
                texture_transparent = (bool)el.value()["transparent"];
                texture_isFoliage = (bool)el.value()["isFoliage"];
                // Add the texture to our texture array
                textureArray.AddTextureToArray(texture_path.c_str());
                // Make an index for this texture
                blocks[el.key()]["index"] = index;
                blocks[el.key()]["group"] = group;
                blocks[el.key()]["transparent"] = texture_transparent;
                blocks[el.key()]["isFoliage"] = texture_isFoliage;
                blocks["" + std::to_string(index) + ""]["transparent"] = texture_transparent;
                blocks["" + std::to_string(index) + ""]["isFoliage"] = texture_isFoliage;
                blocks["" + std::to_string(index) + ""]["group"] = group;
                blocks["" + std::to_string(index) + ""]["texture"] = texture_path;
                count++;
            }
        }
    }

    // Activate our 2D texture array
    textureArray.ActivateShaderArray(cubeShaderProgram.GetID());
}
