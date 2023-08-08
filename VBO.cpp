#include "VBO.hpp"



// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
    // Generate a reference ID for the VBO
    glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}



// Constructor that only generates an ID for the VBO
VBO::VBO()
{
    // Generate a reference ID for the VBO
    glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}



// Bind the VBO
void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}



// Unbind the VBO
void VBO::Unbind()
{
    // Putting a 0 in glBindBuffer unbinds the currently
    // bound buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



// Deletes the VBO
void VBO::Delete()
{
    // Pass in the VBO's reference ID to
    // delete 
	glDeleteBuffers(1, &ID);
}



// Initializes the VBO from blank constructor
void VBO::InitVBO(GLfloat* vertices, GLsizeiptr size)
{
    // Generate a reference ID for the VBO
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}



// Initializes the VBO from blank constructor
void VBO::InitVBO(GLuint* vertices, GLsizeiptr size)
{
    // Generate a reference ID for the VBO
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}


