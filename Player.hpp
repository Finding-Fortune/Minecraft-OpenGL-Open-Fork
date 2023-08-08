#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class Player
{
public:
    Player();  // Constructor

    // Poll events
    void ProcessInput(glm::vec3 playerPosition, glm::vec3 playerOrientation);

private:
    GLFWwindow *window_;

};