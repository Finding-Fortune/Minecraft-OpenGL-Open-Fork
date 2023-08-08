#pragma once

#include "ShaderManager.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

class GUI
{
public:
    GUI();

    void RenderCrosshair();

    Shader guiShaderProgram{"shaders/gui.vert", "shaders/gui.frag"};

private:
    // Buffers for the GUI
    VAO GuiVAO;
    VBO GuiVBO;
};


