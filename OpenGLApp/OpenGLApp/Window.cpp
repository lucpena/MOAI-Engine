#include "Window.h"

using std::cerr;
using std::cout;
using std::endl;

Window::Window()
{
    width = 1280;
    height = 720;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
    width = windowWidth;
    height = windowHeight;
}

int Window::Initialise()
{
    if (!glfwInit())
    {
        printf("Error Initialising GLFW");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW Windows Properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core Profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatiblity
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create the window
    mainWindow = glfwCreateWindow(width, height, "MOAI ENGINE", nullptr, nullptr);
    if (!mainWindow)
    {
		cerr << "\n\nERROR: Failed to create WINDOW.\n" << endl;        
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set the current context
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension access
    glewExperimental = GL_TRUE;

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        cerr << "\n\nERROR: Failed to initialize GLEW.\n\n" << endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Create Viewport
    glViewport(0, 0, bufferWidth, bufferHeight);
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}
