#include "Camera.hpp"

#include <cmath> // ceil



// Initialize a camera
Camera::Camera(GLint width, GLint height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}



// Update the FOV, etc of the camera
void Camera::updateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane)
{
	// Makes camera look in the right direction from the right position
	viewMatrix = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (GLfloat)width / height, nearPlane, farPlane);
	// Orthographic matrix
	orthographicMatrix = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);//glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, nearPlane, farPlane);
}



// Exports camera matrix to the shaders
void Camera::Matrix(Shader& shader, const char* projectionUniformName, const char* viewUniformName)
{
	// Make sure we use the shader program
	glUseProgram(shader.GetID());
	// Set our Camera matrix in our shaders
	glUniformMatrix4fv(glGetUniformLocation(shader.GetID(), projectionUniformName), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetID(), viewUniformName), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	// Clear the shader program after we are done using it
	glUseProgram(0);
}



// Exports orthographic matrix to shaders
void Camera::OrthographicMatrix(Shader& shader, const char* orthographicUniformName)
{
	// Make sure we use the shader program
	glUseProgram(shader.GetID());
	// Set our orthographic matrix in our shaders
	glUniformMatrix4fv(glGetUniformLocation(shader.GetID(), orthographicUniformName), 1, GL_FALSE, glm::value_ptr(orthographicMatrix));
	// Clear the shader program after we are done using it
	glUseProgram(0);
}



// Handles camera/movement inputs and mouse inputs
void Camera::Inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 1.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}



	// Handles mouse inputs
	if (!showMouse)
	{
		// Hides mouse cursor so you can look around without
		// cursor in the way
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		GLfloat rotX = sensitivity * (GLfloat)(mouseY - (height / 2)) / height;
		GLfloat rotY = sensitivity * (GLfloat)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(91.5f)) < glm::radians(90.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}

	// If user presses the M key, the user can then toggle mouse movability
	// timer sets a 1 second delay between mouse toggles
	if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetTime() - mouseToggleTimer > 1)
	{
		// Reset timer
		mouseToggleTimer = glfwGetTime();
		showMouse = !showMouse; // Toggle the mouse key
		if(showMouse)
		{
			// Unhides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		std::cout << "Mouse " << (showMouse == true ? "Enabled" : "Disabled") << std::endl;
	}
}



// Prints position of the camera
void Camera::PrintPosition()
{
	if (prevX != round(Position.x) || prevY != round(Position.y) || prevZ != round(Position.z))
	{
		std::cout << "Position: " << round(Position.x) << " " << round(Position.y) << " " << round(Position.z) << std::endl;
		prevX = round(Position.x);
		prevY = round(Position.y);
		prevZ = round(Position.z);
	}
}



// Returns position of the camera as a string
std::string Camera::GetPosition()
{
	return std::to_string((GLint)(Position.x)) + std::string("x ") + std::to_string((GLint)(Position.y)) + std::string("y ") + std::to_string((GLint)(Position.z)) + std::string("z");
}



// Returns orientation of the camera as a string
std::string Camera::GetOrientation()
{
	return std::to_string(ceil(Orientation.x * 100) / 100) + std::string("x ") + std::to_string(ceil(Orientation.y * 100) / 100) + std::string("y ") + std::to_string(ceil(Orientation.z * 100) / 100) + std::string("z");
}


