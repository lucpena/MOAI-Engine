// C++ libraries
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>

// OpenGL Libraries
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// My Libraries
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

//-------------------------------------------------------------------------

using std::cout;
using std::endl;
using std::cerr;
using std::vector;

const float toRadians = 3.14159265f / 180.0f;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.5f;
float triIncrement = 0.0005f;

float currentAngle = 0.0f;



Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

// Function to create a simple triangle in OpenGL
void CreateObjects()
{
	uint32_t indices[] = 
	{
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	// Define the vertex data for the triangle
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f	
	};

    Mesh* triangleMesh = new Mesh();
    triangleMesh->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(triangleMesh);

    Mesh *triangleMesh2 = new Mesh();
    triangleMesh2->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(triangleMesh2);
}

void CreateShaders()
{
	Shader *_shader = new Shader();
	_shader->CreateFromFile(vShader, fShader);
	shaderList.push_back(*_shader);
}

// Pressing a button
void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mode )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{
		glfwSetWindowShouldClose( window, GL_TRUE );
	}
}

// Main function for the OpenGL application
int main()
{

	// Create the main window
	mainWindow = Window(1280, 720);
	mainWindow.Initialise();

	// Create the triangle and set up the shaders
	CreateObjects();
	CreateShaders();

	GLuint uniformProjection = 0, uniformModel = 0;

    // Setting Projection matrix
    glm::mat4 projection = glm::perspective(45.0f, 
										   (GLfloat)mainWindow.getBufferWidth()/(GLfloat)mainWindow.getBufferHeight(), 
										   0.1f, 
										   500.0f);

	// Render loop: keeps the window open until the user closes it
	while (!mainWindow.getShouldClose())
	{
		// Poll for and process events like keyboard input
		glfwPollEvents();

		if( direction )
		{
			triOffset += triIncrement;
		} else 
		{
			triOffset -= triIncrement;
		}

		if( abs(triOffset) >= triMaxOffset ) 
		{
			direction = !direction;
		}

		currentAngle += 0.01f;
		if( currentAngle >= 360 )
		{
			currentAngle = 0;
		}

		// Clear the screen with a specific color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program for rendering
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();

		// Defining the model matrix for the first piramyd
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, -4.0f));

		model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

        // Attach the Model matrix
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Attach the Projection Matrix 
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        // Rendering the mesh
        meshList[0]->RenderMesh();

        // Defining the model matrix for the second piramyd
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -4.0f));
        model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.5f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        // Stop using the shader program
		glUseProgram(0);

		// Swap the front and back buffers to display the rendered frame
		mainWindow.swapBuffers();
	}

	// Terminate GLFW and release its resources
	glfwTerminate();

	return 0;
}
