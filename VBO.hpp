#pragma once

#include <glad/glad.h>



class VBO
{
public:
    // Reference ID of the Vertex Buffer Object
	GLuint ID;
	
    // Constructor that generates a Vertex Buffer Object and links it to vertices
    VBO(GLfloat* vertices, GLsizeiptr size);
	// Blank constructor
	VBO();

	// Initialize VBO from blank constructor
    void InitVBO(GLfloat* vertices, GLsizeiptr size);
	// Initialize VBO from blank constructor
    void InitVBO(GLuint* vertices, GLsizeiptr size);
    // Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};