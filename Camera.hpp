#pragma once

#include "ShaderManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string> // for string class



class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position; // Position of the camera
	glm::vec3 Orientation = glm::vec3(1.0f, 0.0f, -1.0f); // Default orientation of the camera; which direction looking at

	// Camera constructor to set up initial values
	Camera(GLint width, GLint height, glm::vec3 position);

	// Updates the camera matrix to the Vertex Shader
	void updateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* projectionUniformName, const char* viewUniformName);
	// Exports orthographic matrix to the Vertex Shader
	void OrthographicMatrix(Shader& shader, const char* orthographicUniformName);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
	// Print the camera's position
	void PrintPosition();
	// Gets the camera's position as a string
	std::string GetPosition();
	// Gets the camera's orientation as a string
	std::string GetOrientation();

private:
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); // The up direction
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // Initialize identity matrix for projection
	glm::mat4 viewMatrix = glm::mat4(1.0f); // Initialize identity matrix for view
	glm::mat4 orthographicMatrix = glm::mat4(1.0f); // Initialize identity matrix for orthographic

	// Prevents the camera from jumping around when first clicking left click
	GLboolean firstClick = true;
	// Whether the mouse should be visible, toggleable
	GLboolean showMouse = false;
	// Stores the time before previous mouse click for showMouse toggle
	GLfloat mouseToggleTimer = 0;

	// Stores the width and height of the window
	GLint width;
	GLint height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	GLfloat speed = 0.1f;
	GLfloat sensitivity = 100.0f;

	// Variables used to track camera's position
	GLint prevX = round(Position.x);
	GLint prevY = round(Position.y);
	GLint prevZ = round(Position.z);
};


