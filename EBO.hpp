#pragma once

#include <glad/glad.h>



class EBO
{
public:
    // Reference ID of the Element Buffer Object
    GLuint ID;

    // Constructor that generates an Elements Buffer Object 
    // and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);
    // Constructor that generates an Elements Buffer Object 
    // and does not link it to indices
	EBO(GLsizeiptr size);
    // Constructor that only generates an ID for the EBO
    EBO(){};

    // Initialize EBO from blank constructor
    void InitEBO(GLuint* indices, GLsizeiptr size);
    // Binds the EBO
    void Bind();
    // Unbinds the EBO
    void Unbind();
    // Deletes the EBO
    void Delete();
};