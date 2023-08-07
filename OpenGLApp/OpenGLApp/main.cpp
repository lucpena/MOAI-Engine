#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>

//#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// Use glm::mat4 model(1.0f);
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"

using std::cout;
using std::endl;
using std::cerr;
using std::vector;

const GLint WIDTH = 1280, HEIGHT = 720;
const float toRadians = 3.14159265f / 180.0f;

GLuint shader, uniformModel, uniformProjection;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.5f;
float triIncrement = 0.0005f;

float currentAngle = 0.0f;

vector<Mesh*> meshList;

// Vertex Shader
static const char *vShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
out vec4 vColor;		            										  \n\
                                                                              \n\
uniform mat4 model;                                                           \n\
uniform mat4 projection;                                                       \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = projection * model * vec4(pos, 1.0);		  				  \n\
	vColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);							  \n\
}";

// Fragment Shader
static const char* fShader = " \n\
#version 330  \n\
 \n\
out vec4 colour;  \n\
\n\
in vec4 vColor; \n\
\n\
void main() {  \n\
 \n\
	colour = vColor; \n\
 \n\
}";

// Function to compile and attach a shader to a shader program
// Parameters:
// - theProgram: ID of the shader program to which the shader will be attached
// - shaderCode: C-style string containing the source code of the shader
// - shaderType: Type of the shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType)
{
	// Create a new shader object of the specified type
	GLuint theShader = glCreateShader(shaderType);

	// Provide the shader source code to the shader object
	const GLchar *theCode[1];
	theCode[0] = shaderCode;

	// Specify the length of the source code for the shader
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	// Associate the source code with the shader object
	glShaderSource(theShader, 1, theCode, codeLength);

	// Compile the shader source code into a binary format that can be executed on the GPU
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = {0};

	// Check if the shader compilation was successful
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		// If compilation failed, get the error log and display it along with the shader type
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		cerr << "\n\nERROR: Failed to compile the " << ((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment") << " shader: " << eLog << endl;
		return;
	}

	// If the shader compilation was successful, attach the shader to the shader program
	glAttachShader(theProgram, theShader);
}

// Function to compile and link vertex and fragment shaders into a shader program
void CompileShaders()
{
	// Create a new shader program and get its ID
	shader = glCreateProgram();

	// Check if the shader program was created successfully
	if (!shader)
	{
		cerr << "\n\nERROR: Failed to create shader program.\n" << endl;
		return;
	}

	// Attach and compile the vertex shader to the shader program
	AddShader(shader, vShader, GL_VERTEX_SHADER);

	// Attach and compile the fragment shader to the shader program
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Link the shader program to create the final executable in the GPU
	glLinkProgram(shader);

	// Check if the linking was successful
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		// If linking failed, get the error log and display it
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cerr << "\n\nERROR: Failed to link program: " << eLog << endl;
		return;
	}

	// Validate the shader program to check for potential issues
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		// If validation failed, get the error log and display it
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cerr << "\n\nERROR: Failed to validate program: " << eLog << endl;
		return;
	}

	// Shader compilation and linking successful
	// The shader program is now ready for use in rendering.

    // Getting the variables from the Shaders
    uniformModel = glGetUniformLocation(shader, "model");
    uniformProjection = glGetUniformLocation(shader, "projection");
}

// Function to create a simple triangle in OpenGL
void CreateTriangle()
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
	// Initialize GLFW (Graphics Library Framework) for window management
	if (!glfwInit())
	{
		cerr << "\n\nERROR: Failed to initialize GLFW.\n" << endl;
		glfwTerminate();
		return -1;
	}

	// Set the required OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create the application window with the specified dimensions and title
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "MOAI Engine", nullptr, nullptr);

	// Set the key callback function to handle keyboard input
	glfwSetKeyCallback(window, KeyCallback);

	// Check if the window creation was successful
	if (window == nullptr)
	{
		cerr << "\n\nERROR: Failed to create GLFW Window.\n\n" << endl;
		glfwTerminate();
		return -1;
	}

	// Make the current OpenGL context active for rendering
	glfwMakeContextCurrent(window);

	// Initialize GLEW (OpenGL Extension Wrangler Library) for accessing OpenGL functions
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "\n\nERROR: Failed to initialize GLEW.\n\n" << endl;
		return -1;
	}

	// Enable Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Set the viewport dimensions to match the window size
	glViewport(0, 0, WIDTH, HEIGHT);

	// Create the triangle and set up the shaders
	CreateTriangle();
	CompileShaders();

    // Setting Projection matrix
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 500.0f);

	// Render loop: keeps the window open until the user closes it
	while (!glfwWindowShouldClose(window))
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
		glUseProgram(shader);

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
		glfwSwapBuffers(window);
	}

	// Terminate GLFW and release its resources
	glfwTerminate();

	return 0;
}
