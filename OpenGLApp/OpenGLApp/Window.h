#pragma once

#include <iostream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

class Window
{
public:
    Window();

    Window(GLint windowWidth, GLint windowHeight);

    GLFWwindow* Initialise(string windowName);

    GLint getBufferWidth() { return bufferWidth; }
    GLint getBufferHeight() { return bufferHeight; }

    bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

    bool *getsKeys() { return keys; }
    GLfloat getXChange();
    GLfloat getYChange();

    void swapBuffers() { glfwSwapBuffers(mainWindow); }

    ~Window();

private:
    GLFWwindow *mainWindow;

    GLint width, height;
    GLint bufferWidth, bufferHeight;

    bool keys[1024];

    GLfloat lastX;
    GLfloat lastY;
    GLfloat xChange;
    GLfloat yChange;
    bool mouseFirstMoved;

    void createCallbacks();
    static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
    static void handleMouse(GLFWwindow *window, double xPos, double yPos);

};

