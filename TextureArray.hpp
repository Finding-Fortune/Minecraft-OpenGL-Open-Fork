#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class TextureArray
{
public:
    GLuint textureID;

    TextureArray();

    void AddTextureToArray(const GLchar *filePath);
    void ActivateShaderArray(GLuint shaderProgramID);
    void Bind();
    void Unbind();
    void Delete();

private:
    glm::vec2 image_size = glm::vec2(512, 512);
    GLchar textureArrayName[15] = "ourTexture";
    GLuint textureUnitID;
    GLuint image_index = 0;
};

// Increments for each texture array instance that is created. This keeps track of
// which texture slots we have activated so far
inline GLuint textureSlotIndex = 0;
