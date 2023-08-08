#include "GUI.hpp"
#include "WorldConstants.hpp"



// Vertex information for GUI
GLfloat crosshairVertices[] = {
    // Horizontal crosshair line.
    -World::crosshairLength, -World::crosshairThickness,  // bottom-left
     World::crosshairLength, -World::crosshairThickness,  // bottom-right
     World::crosshairLength,  World::crosshairThickness,  // top-right
     World::crosshairLength,  World::crosshairThickness,  // top-right
    -World::crosshairLength,  World::crosshairThickness,  // top-left
    -World::crosshairLength, -World::crosshairThickness,  // bottom-left

    // Vertical crosshair line. 
    // The 1350 and 850 are the width and height of the window.
    // We multiply by it so that we scale the vertical line by the window resolution
    // Make the horizontal and vertical crosshairs the same size
    -World::crosshairThickness, -World::crosshairLength * ((GLfloat)1350 / 850),  // bottom-left
     World::crosshairThickness, -World::crosshairLength * ((GLfloat)1350 / 850),  // bottom-right
     World::crosshairThickness,  World::crosshairLength * ((GLfloat)1350 / 850),  // top-right
     World::crosshairThickness,  World::crosshairLength * ((GLfloat)1350 / 850),  // top-right
    -World::crosshairThickness,  World::crosshairLength * ((GLfloat)1350 / 850),  // top-left
    -World::crosshairThickness, -World::crosshairLength * ((GLfloat)1350 / 850),  // bottom-left
}; 



GUI::GUI() : guiShaderProgram("shaders/gui.vert", "shaders/gui.frag")  // Member-Initializer List
{
    GuiVAO.Bind();
    GuiVBO.Bind();
	// Links VBO attributes such as coordinates and colors to VAO
    GuiVBO.InitVBO(crosshairVertices, sizeof(crosshairVertices));
	GuiVAO.LinkAttrib(GuiVBO, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0);
	// Unbind all to prevent accidentally modifying them
	GuiVBO.Unbind();
    GuiVAO.Unbind();
}



void GUI::RenderCrosshair()
{
    // Render the crosshair
    GuiVAO.Bind();
    guiShaderProgram.Activate();
    glDrawArrays(GL_TRIANGLES, 0, sizeof(crosshairVertices) / sizeof(GLfloat) / 2);
    GuiVAO.Unbind();
}


