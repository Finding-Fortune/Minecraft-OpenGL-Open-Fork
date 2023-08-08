#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>



// Get file contents of a shader file
std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	// Blank constructor
	Shader();

	// Set the shader vertexFile and fragmentFile
	void setShaderFiles(const char* vertexFile, const char* fragmentFile);
	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();
	// Get ID of the shader program
	GLuint GetID();
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(GLuint shader, const char* type);
	// Reference ID of the Shader Program
	GLuint ID;
};
