#include "WindowManager.hpp"
#include "BufferManager.hpp"
#include "Camera.hpp"
#include "GUI.hpp"
#include "Sky.hpp"
#include "Player.hpp"

// Math headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>



int main()
{
	// Initialize Classes
    WindowManager &window = WindowManager::Instance();
	BufferManager &bufferManager = BufferManager::Instance();
    Camera camera(window.GetResolution().x, window.GetResolution().y, glm::vec3(0.0f, 0.0f, 0.0f)); // Camera width, camera height, camera starting position
    GUI guiManager;
    Sky skyManager;
    Player player;

    glEnable(GL_CULL_FACE);  // Enable culling of back faces

    // Enable blending for transparent blocks
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render Loop
    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        // Handles camera inputs
		camera.Inputs(window.GetWindow());
		// Updates the camera matrix to the Vertex Shader
        camera.Matrix(bufferManager.cubeShaderProgram, "projectionMatrix", "viewMatrix");
        camera.Matrix(bufferManager.lightShaderProgram, "projectionMatrix", "viewMatrix");
        camera.OrthographicMatrix(guiManager.guiShaderProgram, "orthographicMatrix");
        // Update the FOV in the matrix. FOV degree, near and far clip planes
		camera.updateMatrix(45.0f, 0.1f, World::BlockRenderDistance); 
        // Set sky colors
        skyManager.SetSkyColor();
        // Run all of our buffer business, including chunk/mesh rendering
		bufferManager.RunLoop(window.GetWindow(), camera.Position, camera.Orientation);
        // Process user input
        player.ProcessInput(camera.Position, camera.Orientation);
        // Run Gui Business
        guiManager.RenderCrosshair();
        // Error logging
        window.CheckErrors();
        // Set FPS and camera coordinates to GLFW window title
        window.SetWindowTitle("FPS: " + std::to_string(window.GetFPS()) + "             Camera Position: " + camera.GetPosition() + "             Camera Orientation: " + camera.GetOrientation());

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window.GetWindow());
        // Poll for events so window responds to clicks and such
        glfwPollEvents();
    }

    return 0;
}


