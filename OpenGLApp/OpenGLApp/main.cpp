#include <iostream>
#include <string.h>

//#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

using std::cout;
using std::endl;
using std::cerr;

const GLint WIDTH = 800, HEIGHT = 600;

/***********************************************************
 	VAO => Vertex Array Objects
	VBO => Vertex Buffer Object

	VAO defines WHAT the data a vertex has.
	VBO defines the DATA itself

***********************************************************/
GLuint VAO, VBO;
/**********************************************************/

GLuint shader;

// Vertex Shader
static const char* vShader = " \n\
#version 330 \n\
 \n\
layout(location = 0) in vec3 pos; \n\
 \n\
void main() { \n\
 \n\
	gl_Position = vec4(0.5 * pos.x, 0.5 * pos.y, 0.5 * pos.z, 1.0); \n\
 \n\
}";

// Fragment Shader
static const char* fShader = " \n\
#version 330  \n\
 \n\
out vec4 colour;  \n\
 \n\
void main() {  \n\
 \n\
	colour = vec4( 1.0, 1.0, 0.0, 1.0); \n\
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
}

// Function to create a simple triangle in OpenGL
void CreateTriangle()
{
	// Define the vertex data for the triangle
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, // Vertex 1: x, y, z coordinates
		1.0f, -1.0f, 0.0f,	// Vertex 2: x, y, z coordinates
		0.0f, 1.0f, 0.0f	// Vertex 3: x, y, z coordinates
	};

	// Allocates GPU memory space for 1 Vertex Array object (VAO)
	glGenVertexArrays(1, &VAO);

	// Bind the newly created VAO, all subsequent operations will be performed on this VAO
	glBindVertexArray(VAO);

	// Creates a Vertex Buffer Object (VBO) inside the VAO to store the vertex data
	glGenBuffers(1, &VBO);

	// Bind the VBO to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Transfer the vertex data to the GPU memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Define the layout of the vertex data within the VBO
	// Attribute 0 (index 0) corresponds to the position attribute of the vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable the vertex attribute at index 0 (position attribute)
	glEnableVertexAttribArray(0);

	// Unbind the VBO (not necessary, but good practice to unbind when done)
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO (not necessary, but good practice to unbind when done)
	glBindVertexArray(0);
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

	// Set the viewport dimensions to match the window size
	glViewport(0, 0, WIDTH, HEIGHT);

	// Create the triangle and set up the shaders
	CreateTriangle();
	CompileShaders();

	// Render loop: keeps the window open until the user closes it
	while (!glfwWindowShouldClose(window))
	{
		// Poll for and process events like keyboard input
		glfwPollEvents();

		// Clear the screen with a specific color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the shader program for rendering
		glUseProgram(shader);

		// Bind the vertex array object containing the triangle data
		glBindVertexArray(VAO);

		// Draw the triangle using OpenGL draw call
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Unbind the vertex array object
		glBindVertexArray(0);

		// Stop using the shader program
		glUseProgram(0);

		// Swap the front and back buffers to display the rendered frame
		glfwSwapBuffers(window);
	}

	// Terminate GLFW and release its resources
	glfwTerminate();

	return 0;
}
