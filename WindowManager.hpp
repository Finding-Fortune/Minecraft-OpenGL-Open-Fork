#pragma once

#define GLFW_INCLUDE_NONE
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <string> // for string class



class WindowManager
{
public:
    // Singleton Design
    static WindowManager &Instance()
    {
        static WindowManager instance;
        return instance;
    }
    WindowManager();  // Constructor
    ~WindowManager(); // Destructor

    // Getters
    GLFWwindow *GetWindow() const;
    // Get our window resolution
    glm::ivec2 GetResolution();
    // Get our FPS
    GLdouble GetFPS();
    // Print our FPS
    void printFPS();
    // Checks for any OpenGL errors
    void CheckErrors();

    // Setter
    void SetWindowTitle(const std::string &title);

private:
	GLFWwindow *window_;
    glm::ivec2 resolution = glm::ivec2(1350, 850);
    GLdouble fps; // frames per second
    GLint ErrorCode;  // Integer to track error codes

    // window resize callback
    static void framebuffer_size_callback(GLFWwindow *window, GLint width, GLint height);
};