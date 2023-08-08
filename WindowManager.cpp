#include "WindowManager.hpp"
#include "WorldConstants.hpp"



// Window constructor
WindowManager::WindowManager()
{
    std::cout << "Resolution: " << resolution.x << " " << resolution.y << std::endl;
    glfwInit();

    // Tell glfw what version of openGL we are using
    // we are using openGL 3.3, so major = 3 and minor = 3 (the .3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // The profile is the package of functions openGL provides
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); 

    // If this is macos then we need to enable forward compatibility
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create a window
    // Width, height, name of window, fullscreen or not, not important
    window_ = glfwCreateWindow(resolution.x, resolution.y, "LearnOpenGL", NULL, NULL);
    // Error checking if awindow fais to create
    if (window_ == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Use the new window
    glfwMakeContextCurrent(window_);
    // Callback function for when window is resized
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
     // Have Glad load up needed configs for openG:
    gladLoadGL();

    // Tell glad viewport of our window
    #ifdef __APPLE__
        glViewport(0, 0, resolution.x * 2, resolution.y * 2); // Due to mac glitch resolution needs to be twice as big
    #else
        glViewport(0, 0, resolution.x, resolution.y);
    #endif
    // Set aspect ratio of our window so when window is resized,
    // the geometry does not stretch
    glfwSetWindowAspectRatio(window_, 16, 9);
    // Set color of screen. Parameters are red, green, blue, alpha (transparency)
    glClearColor(World::skyColor.x, World::skyColor.y, World::skyColor.z, World::skyColor.w);
    // Clean the back buffer and depth buffer 
    // and assign the new color to back buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth testing so openGL can figure out which
    // shape is in front of each other
    glEnable(GL_DEPTH_TEST);
    // Swap the buffer to the new colored buffer
    glfwSwapBuffers(window_);

    std::cout << "Program Initialized" << std::endl;
}



// Get our window object
GLFWwindow *WindowManager::GetWindow() const
{
	return window_;
}



// Window destructor
WindowManager::~WindowManager()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
    std::cout << "Program Terminated" << std::endl;
}



glm::ivec2 WindowManager::GetResolution()
{
    return resolution;
}



// glfw: whenever the window size changed (by OS or user resize) 
// this callback function executes
void WindowManager::framebuffer_size_callback (GLFWwindow *window, GLint width, GLint height)
{
    #ifdef __APPLE__
        // Make sure the viewport matches the new window dimensions;
        std::cout << "Width: " << (width/2) << " Height: " << (height/2) << std::endl;
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    #else
        // Make sure the viewport matches the new window dimensions;
        std::cout << "Width: " << width << " Height: " << height << std::endl;
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    #endif
}



void WindowManager::printFPS()
{
    static GLdouble previous_seconds = glfwGetTime();
    static GLint frame_count;
    GLdouble current_seconds = glfwGetTime();
    GLdouble elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25)
    {
        previous_seconds = current_seconds;
        GLdouble fps = (GLdouble)frame_count / elapsed_seconds;
        std::cout << "FPS: " << fps << std::endl;
        frame_count = 0;
    }
    frame_count++;
}



GLdouble WindowManager::GetFPS()
{
    static GLdouble previous_seconds = glfwGetTime();
    static GLint frame_count;
    GLdouble current_seconds = glfwGetTime();
    GLdouble elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25)
    {
        previous_seconds = current_seconds;
        fps = (GLdouble)frame_count / elapsed_seconds;
        frame_count = 1;
        return fps;
    }
    frame_count++;
    return fps;
}



void WindowManager::SetWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(window_, title.c_str());
}



// Checks for any OpenGL errors
void WindowManager::CheckErrors()
{
    // Get our current error code
    ErrorCode = glGetError();
    // If the error code is not 0, then there is an error
    if (ErrorCode != 0)
    {
        switch (ErrorCode) {
            case 1280:
                std::cout << "GL_INVALID_ENUM. Enumeration parameter is not legal." << std::endl;
                break;
            case 1281:
                std::cout << "GL_INVALID_VALUE. Value parameter is not legal." << std::endl;
                break;
            case 1282:
                std::cout << "GL_INVALID_OPERATION. State for a command is not legal for its given parameters." << std::endl;
                break;
            case 1283:
                std::cout << "GL_STACK_OVERFLOW. Command would cause a stack overflow." << std::endl;
                break;
            case 1284:
                std::cout << "GL_STACK_UNDERFLOW. Command would cause a stack underflow." << std::endl;
                break;
            case 1285:
                std::cout << "GL_OUT_OF_MEMORY. Not enough memory left to execute command." << std::endl;
                break;
            case 1286:
                std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION. Command is not allowed on the current incomplete framebuffer." << std::endl;
                break;
            default:
                std::cout << "Unknown error." << std::endl;
                break;
        }
    }
}


