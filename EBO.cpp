#include "EBO.hpp"



// Constructor that generates an 
// Elements Buffer Object and links it to indices
EBO::EBO(GLuint* indices, GLsizeiptr size)
{
    // Generate a reference ID for the EBO
    glGenBuffers(1, &ID);
    // Bind the EBO with the ID we created
    // to the element array buffer OpenGL
    // uses for EBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    // Put the indices data into the EBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}



// Constructor that generates an 
// Elements Buffer Object and does not link it to indices
EBO::EBO(GLsizeiptr size)
{
    // Generate a reference ID for the EBO
    glGenBuffers(1, &ID);
    // Bind the EBO with the ID we created
    // to the element array buffer OpenGL
    // uses for EBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    // Put the indices data into the EBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}



// Binds the EBO
void EBO::Bind()
{
    // Bind the EBO again when needed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}



// Unbinds the EBO
void EBO::Unbind()
{
    // Unbind the EBO when needed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



// Deletes the EBO
void EBO::Delete()
{
    // Delete the EBO
    glDeleteBuffers(1, &ID);
}



// Initialize the EBO from blank constructor
void EBO::InitEBO(GLuint* indices, GLsizeiptr size)
{
    // Generate a reference ID for the EBO
    glGenBuffers(1, &ID);
    // Bind the EBO with the ID we created
    // to the element array buffer OpenGL
    // uses for EBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    // Put the indices data into the EBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}


