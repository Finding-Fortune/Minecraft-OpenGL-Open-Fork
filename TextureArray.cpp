#include "TextureArray.hpp"

#include <stb/stb_image.h> // Used for textures / image processing
#include <iostream>



TextureArray::TextureArray()
{
    // Generate a texture ID
    glGenTextures(1, &textureID);
    // Activate the nth texture unit slot
    glActiveTexture(GL_TEXTURE0 + textureSlotIndex);
    // Bind our texture ID to our 2D texture array and initialize it
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, image_size.x, image_size.y, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Settings for 2D texture array
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Increase the texture slot index, so the next time we create a texture array it does not 
    // use the previous texture slot and override it
    textureUnitID = textureSlotIndex;
    textureSlotIndex++;
}



void TextureArray::AddTextureToArray(const GLchar *filePath)
{
    // Load and generate the texture
    GLint width, height, nrChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, STBI_rgb_alpha); // Loading as rgb_alpha so transparent pixels are transparent
    // Flip textures on load
    stbi_set_flip_vertically_on_load(false);
    if (data)
    {
        // Put data into the 2D texture array
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, image_index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);  
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    else
    {
        std::cout << "Failed to load texture " << filePath << std::endl;
    }

    image_index++;
}



void TextureArray::ActivateShaderArray(GLuint shaderProgramID)
{
    // Have to use our program before setting a uniform
    glUseProgram(shaderProgramID);
    glUniform1i(glGetUniformLocation(shaderProgramID, textureArrayName), textureUnitID);
}



void TextureArray::Bind()
{
    // Bind our texture to the GL_TEXTURE_2D target
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
}



void TextureArray::Unbind()
{
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
    


void TextureArray::Delete()
{
    glDeleteTextures(1, &textureID);
}


