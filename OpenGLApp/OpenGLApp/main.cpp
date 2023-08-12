#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

//-------------------------------------------------------------------------
Window mainWindow;
Camera camera;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

const float toRadians = 3.14159265f / 180.0f;
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.5f;
float triIncrement = 0.0005f;
float currentAngle = 0.0f;

vector<Mesh*> meshList;
vector<Shader> shaderList;

Texture obamiumTexture;

Material dullMaterial;
Material shinyMaterial;

DirectionalLight ambientLight;

//---------------------------------------------------------------------------

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CalcAverageNormals(uint32_t* indices, uint32_t indiceCount, GLfloat* vertices, uint32_t verticesCount, uint32_t vLength, uint32_t normalOffset)
{
	for( size_t i = 0; i < indiceCount; i += 3 )
	{
		uint32_t in0 = indices[i] * vLength;
        uint32_t in1 = indices[i + 1] * vLength;
        uint32_t in2 = indices[i + 2] * vLength;

        // The Vector product of two vectors, a and b, is denoted by a × b. Its resultant vector is perpendicular to a and b, in other words, the NORMAL!!! UwU
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset;
        in1 += normalOffset;
        in2 += normalOffset;

        vertices[in0 + 0] += normal.x;
        vertices[in0 + 1] += normal.y;
        vertices[in0 + 2] += normal.z;

        vertices[in1 + 0] += normal.x;
        vertices[in1 + 1] += normal.y;
        vertices[in1 + 2] += normal.z;

        vertices[in2 + 0] += normal.x;
        vertices[in2 + 1] += normal.y;
        vertices[in2 + 2] += normal.z;
    }

    for( size_t i = 0; i < verticesCount / vLength; i++  )
    {
        uint32_t nOffeset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffeset], vertices[nOffeset + 1], vertices[nOffeset + 2]);
        vec = glm::normalize(vec);

        vertices[nOffeset + 0] = vec.x;
        vertices[nOffeset + 1] = vec.y;
        vertices[nOffeset + 2] = vec.z;
    }
}

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
	GLfloat vertices[] = 
	{
	//   X      Y     Z       U     V    nX    nY    nZ
		-1.0f, -1.0f, 0.6f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 2.0f,   0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,   1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,    0.5f, 1.0f,  0.0f, 0.0f, 0.0f
	};

    CalcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* triangleMesh = new Mesh();//    vertices, indices
    triangleMesh->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(triangleMesh);

    Mesh *triangleMesh2 = new Mesh();//    vertices, indices
    triangleMesh2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(triangleMesh2);
}

void CreateShaders()
{
	Shader *_shader = new Shader();
	_shader->CreateFromFile(vShader, fShader);
	shaderList.push_back(*_shader);
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

	// Define the Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);

	// Load the Texture
	obamiumTexture = Texture("Assets/Textures/obamium.png");
	obamiumTexture.LoadTexture();

	// Setting up Ambient Light
	ambientLight = DirectionalLight(1.0f, 1.0f, 1.0f, 	// RGB Color
									0.18f, 0.75f,			// Ambient Intensity, Diffuse Intensity
									0.0f, 0.0f, -1.0f); // XYZ Direction

	// Setting the materials for Phong Shading
	shinyMaterial = Material(0.5f, 32);
	dullMaterial = Material(0.05f, 2);

	// Getting the Uniforms (Shaders variables)
	// The camera matrices from the shaders
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	
	// Light uniforms
	GLuint uniformAmbientIntensity = 0, uniformAmbientColour = 0;
    GLuint uniformDirection = 0, uniformDiffuseIntensity = 0;
	GLuint uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

	// Setting Projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Render loop: keeps the window open until the user closes it
	while (!mainWindow.getShouldClose())
	{
		// Getting DELTA TIME
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		// Handling the Camera
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
        uniformDirection = shaderList[0].GetDirectionLocation();
        uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		// Setting up the Light
		ambientLight.UseLight(uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection);

		// Setting the Projection Matrix
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Setting the View Matrix
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// Attaching the camera position to the Eye Position for the shaders
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Defining the model matrix for the first piramyd
		glm::mat4 model(1.0f);

		// Translate, Rotate and Scale
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -4.0f));
		model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, 0.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

        // Attach the Model matrix
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// Attach the Texture
		obamiumTexture.UseTexture();

		// Set the Material for the mesh
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// Rendering the mesh
        meshList[0]->RenderMesh();

        // Defining the model matrix for the second piramyd
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -4.0f));
        model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, -0.5f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
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
