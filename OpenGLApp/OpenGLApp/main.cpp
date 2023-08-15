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
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"


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
Texture floorTexture;
Texture plainTexture;

Material dullMaterial;
Material shinyMaterial;
Material veryShinyMaterial;
Material plainMaterial;

DirectionalLight ambientLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model sponza;
Model moai;
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

    for( uint32_t i = 0; i < verticesCount / vLength; i++  )
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

	uint32_t floorIndices[] =  
	{
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = 
	{
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.f, 10.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

    CalcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* triangleMesh = new Mesh();//    vertices, indices
    triangleMesh->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(triangleMesh);

    Mesh* triangleMesh2 = new Mesh();//    vertices, indices
    triangleMesh2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(triangleMesh2);

	Mesh* floorMesh = new Mesh();
	floorMesh->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(floorMesh);
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
	obamiumTexture.LoadTextureA();

	floorTexture = Texture("Assets/Textures/ground_01.png");
	floorTexture.LoadTextureA();

	plainTexture = Texture("Assets/Textures/plain.png");
	plainTexture.LoadTextureA();

	// Setting up Ambient Light
	ambientLight = DirectionalLight(0.63f, 0.75f, 0.90f, // RGB Color
									0.2f, 0.1f,			 // Ambient Intensity, Diffuse Intensity
									0.0f, 0.0f, -1.0f);	 // XYZ Direction

	// Setting Point Lights
	uint32_t pointLightCount = 0;

	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,		// RGB Color
							  	0.4f, 0.3f,				// Ambient Intensity, Diffuse Intensity
								0.0f, 2.0f, -7.0f,		// XYZ Position
								0.3f, 0.2f, 0.1f);		// Constant, Linear, Exponent
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,  // RGB Color
								0.2f, 1.0f,		   // Ambient Intensity, Diffuse Intensity
								0.0f, 5.0f, 5.0f, // XYZ Position
								0.3f, 0.2f, 0.1f); // Constant, Linear, Exponent
	//pointLightCount++;

	// Setting Spot Lights
	uint32_t spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,	  	// RGB Color
							  0.0f, 2.0f,		  	// Ambient Intensity, Diffuse Intensity
							  0.0f, 0.0f, 0.0f,		// XYZ Position
							  0.0f, 0.0f, 0.0f,	 // XYZ Direction
							  0.3f, 0.2f, 0.1f, 
							  20.0f  ); 			// Edge angle
	//spotLightCount++;
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
							  1.0f, 1.0f,
							  0.0f, 5.0f, 0.0f,
							  0.0f, -1.0f, 0.0f,
							  0.3f, 0.2f, 0.1f,
							  20.0f);
	//spotLightCount++;

	// Setting the materials for Phong Shading
	veryShinyMaterial = Material(4.0f, 256);
	shinyMaterial = Material(0.5f, 32);
	dullMaterial = Material(0.05f, 2);

	// Setting the models
	sponza = Model();
	sponza.LoadModel("Assets/Models/Sponza/sponza.obj", "sponza");

	moai = Model();
	moai.LoadModel("Assets/Models/Moai/moai.obj", "moai");

	// Getting the Uniforms (Shaders variables)
	// The camera matrices from the shaders
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	
	// Light uniforms
	GLuint uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

	// Setting Projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Render loop: keeps the window open until the user closes it
	while (!mainWindow.getShouldClose())
	{
		// Getting DELTA TIME
		GLfloat now = (GLfloat)glfwGetTime();
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
		glClearColor(0.63f, 0.75f, 0.90f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program for rendering
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		// Setting the flashlight
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Setting up the Light
		shaderList[0].SetDirectionalLight(&ambientLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

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
        //meshList[0]->RenderMesh();

        // Defining the model matrix for the second piramyd
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -4.0f));
        model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, -0.5f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[1]->RenderMesh();

		// Addind the Floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		floorTexture.UseTexture();
		// plainTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[2]->RenderMesh();

		// Adding the SPONZA model
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5f, -1.0f, -8.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		// model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		sponza.RenderModel();

		// Adding MOAI Model
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.25f, -7.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		moai.RenderModel();

		// Stop using the shader program
		glUseProgram(0);

		// Swap the front and back buffers to display the rendered frame
		mainWindow.swapBuffers();
	}

	// Terminate GLFW and release its resources
	glfwTerminate();

	return 0;
}
