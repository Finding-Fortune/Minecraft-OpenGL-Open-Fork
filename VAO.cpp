#include "VAO.hpp"



// Constructor that generates a VAO ID
VAO::VAO()
{
    // Generate a reference ID for the VAO
    glGenVertexArrays(1, &ID);
}



// Links a VAO attribute such as position 
// or color to the VAO
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
    // Bind the VBO
    VBO.Bind();
    // Link the VAO attribute to the VBO
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Enable the VAO array
    glEnableVertexAttribArray(layout);
    // Unbind the VBO
    VBO.Unbind();
}



// Binds the VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}



// Unbinds the VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}



// Deletes the VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}


