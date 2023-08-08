#pragma once

#include <glad/glad.h>
#include "VBO.hpp"



class VAO
{
public:
    // Reference ID of the Vertex Array Object
    GLuint ID;
   
    // Constructor that generates a Vertex Array Object
    VAO();

    // Links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
    // Binds the VAO
    void Bind();
    // Unbinds the VAO
    void Unbind();
    // Deletes the VAO
    void Delete();
};