#define STB_IMAGE_IMPLEMENTATION

// C++ libraries
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <Windows.h>
#include <thread>

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
#include "SkyBox.h"


using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::thread;

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

float gamma = 2.2f;

// Getting the Uniforms (Shaders variables)
// The camera matrices from the shaders
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

// Light uniforms
GLuint uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0,
uniformOmniLightPos = 0, uniformFarPlane = 0;

vector<Mesh *> meshList;
vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;
Shader postShader;

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
uint32_t spotLightCount = 0;
uint32_t pointLightCount = 0;

SkyBox skybox;

Model sponza;
Model room;
Model briar;
Model formula1;
Model testModel;

//---------------------------------------------------------------------------

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

// Function to create a simple triangle in OpenGL
void CreateObjects()
{

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

	Mesh* floorMesh = new Mesh();
	floorMesh->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(floorMesh);
}

void CreateShaders()
{
	Shader *_shader = new Shader();
	_shader->CreateFromFile(vShader, fShader);
	shaderList.push_back(*_shader);

	directionalShadowShader.CreateFromFile("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFile("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geo", "Shaders/omni_shadow_map.frag");
	postShader.CreateFromFile("Shaders/post-processing.vert", "Shaders/post-processing.frag");
}

void RenderScene()
{
	// // Defining the model matrix for the first piramyd
	glm::mat4 model(1.0f);

	// // Addind the Floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	floorTexture.UseTexture();
	//plainTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	// Adding the SPONZA model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, -1.0f, -8.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	// model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	sponza.RenderModel();

	// Adding the Room
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, -5.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	// model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//   model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//room.RenderModel();

	// Adding the Briar
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -2.0f, -7.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//  model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	briar.RenderModel();

	// Adding Formula 1 Ferrari
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, -2.0f, -5.0f));
	model = glm::scale(model, glm::vec3(0.009f, 0.009f, 0.009f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//   model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	veryShinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	formula1.RenderModel();

	// Adding MOAI Model
	// currentAngle += 0.01f;
	// if (currentAngle >= 360)
	// {
	// 	currentAngle = 0;
	// }

	// model = glm::mat4(1.0f);
	// model = glm::translate(model, glm::vec3(0.0f, -1.36f, -7.0f));
	// model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	// model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	// model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, 0.0f, 0.5f));
	// glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	// dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	// moai.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	// Setting the framebuffer as the same size of the Viewport
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	// Writing the shadow map
	light->GetShadowMap()->Write();

	// Clearing the info already in the Depth Buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// Getting the model location fom the Shader
	uniformModel = directionalShadowShader.GetModelLocation();

	// Calculate the Transform
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	// Validates the shader
	directionalShadowShader.Validate();

	RenderScene();

	// Unbind for the regular Render Pass;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OmniShadowMapPass(PointLight* light)
{
	// Setting the framebuffer as the same size of the Viewport
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	omniShadowShader.UseShader();

	// Getting the model location fom the Shader
	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	// Writing the shadow map
	light->GetShadowMap()->Write();

	// Clearing the info already in the Depth Buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	// Validates the shader
	omniShadowShader.Validate();

	RenderScene();

	// Unbind for the regular Render Pass;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	// Resseting the viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Clear the screen with a specific color
	glClearColor(pow(0.63f, 1.0f / gamma), pow(0.75f, 1.0f / gamma), pow(0.90f, 1.0f / gamma), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rendering the Skybox
	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	// Make shure we are using the right shader
	shaderList[0].UseShader();

	// Use the shader program for rendering
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	// Setting the Projection Matrix
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Setting the View Matrix
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Attaching the camera position to the Eye Position for the shaders
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	// Setting up the Light
	shaderList[0].SetDirectionalLight(&ambientLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&ambientLight.CalculateLightTransform());

	// Setting shadow map
	ambientLight.GetShadowMap()->Read(GL_TEXTURE2);

	// ID of the Texture (It's by default 0 :D)
	shaderList[0].SetTexture(1);

	//
	shaderList[0].SetDirectionalShadowMap(2);

	// Setting the flashlight
	// glm::vec3 lowerLight = camera.getCameraPosition();
	// lowerLight.y -= 0.5f;
	// spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shaderList[0].Validate();

	RenderScene();

}

void PostProcessingPass()
{
	shaderList[0].UseShader();
	shaderList[0].SetInt("screenTexture", 0);
	shaderList[0].SetFloat("gamma", gamma);
}

// Main function for the OpenGL application
int main()
{
	// Makes the console green like The Matrix UwU
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 10); // Set text color to green

	// Sets VSync off
	//glfwSwapInterval(0);

	cerr << "--------------------------------------------------------" << endl;
	cerr << "| MOAI ENGINE v1.0 - OpenGL 3.3                        |" << endl;
	cerr << "--------------------------------------------------------\n\n" << endl;
	cerr << "Loading..." << endl;

	// Create the main window
	mainWindow = Window(WINDOW_WIDTH, WINDOW_HEIGHT);
	mainWindow.Initialise();

	// Create the triangle and set up the shaders
	CreateObjects();
	CreateShaders();

	// Define the Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);

	// Load the Texture
	// obamiumTexture = Texture("Assets/Textures/obamium.png");
	// obamiumTexture.LoadTextureA();

	floorTexture = Texture("Assets/Textures/ground_01.png");
	floorTexture.LoadTexture();

	// plainTexture = Texture("Assets/Textures/plain.png");
	// plainTexture.LoadTextureA();

	// Setting the materials for Phong Shading
	veryShinyMaterial = Material(4.0f, 256);
	shinyMaterial = Material(0.5f, 32);
	dullMaterial = Material(0.05f, 2);

	// Setting the models
	sponza = Model();
	//sponza.LoadModel("Assets/Models/Sponza/sponza.obj", "sponza");

	room = Model();
	//room.LoadModel("Assets/Models/Room/living_room.obj", "room");

	briar = Model();
	briar.LoadModel("Assets/Models/Briar/scene.gltf", "briar");

	formula1 = Model();
	formula1.LoadModel("Assets/Models/FF1/f1.fbx", "ff1");
	
	testModel = Model();
	//testModel.LoadModel("Assets/Models/TestModel/scene.gltf", "testModel");

	// Setting up Ambient Light
	ambientLight = DirectionalLight(2048, 2048,				// Shadow Buffer (width, height)
									1.0f, 1.0f, 1.0f,		// RGB Color
									0.1f, 0.2f,				// Ambient Intensity, Diffuse Intensity
									0.0f, -25.0f, -8.0f);	// XYZ Direction

	// Setting Point Lights
	pointLights[0] = PointLight(1024, 1024, 	   // Shadow Width and Height 
								0.1f, 100.0f,      // Shadow Near and Far Planes
								1.0f, 1.0f, 1.0f,  // RGB Color
								0.9f, 0.8f,		   // Ambient Intensity, Diffuse Intensity
								0.0f, 4.0f, -5.0f, // XYZ Position
								0.3f, 0.2f, 0.1f); // Constant, Linear, Exponent
	//pointLightCount++;

	pointLights[1] = PointLight(1024, 1024,		   // Shadow Width and Height
								0.1f, 100.0f,	   // Shadow Near and Far Planes
								1.0f, 1.0f, 1.0f,  // RGB Color
								0.2f, 1.0f,		   // Ambient Intensity, Diffuse Intensity
								0.0f, 5.0f, 0.0f,  // XYZ Position
								0.8f, 0.2f, 0.1f); // Constant, Linear, Exponent
	//pointLightCount++;

	//Setting Spot Lights
	// spotLights[0] = SpotLight(1024, 1024,		// Shadow Width and Height
	// 						  0.1f, 100.0f,		// Shadow Near and Far Planes
	// 						  0.0f, 0.0f, 0.0f, // RGB Color
	// 						  0.9f, 2.0f,		// Ambient Intensity, Diffuse Intensity
	// 						  0.0f, 0.0f, 5.0f, // XYZ Position
	// 						  0.0f, 0.0f, 0.0f, // XYZ Direction
	// 						  0.3f, 0.2f, 0.1f, // 
	// 						  20.0f); 			// Edge angle
	// spotLightCount++;

	// FLASHLIGHT
	spotLights[0] = SpotLight(1024, 1024,	// Shadow Width and Height
							  0.1f, 100.0f, // Shadow Near and Far Planes
							  0.0f, 0.0f, 0.0f,
							  2.0f, 2.0f,
							  -0.5f, 5.0f, -2.0f,
							  0.0f, -1.0f, -1.25f,
							  0.9f, 0.2f, 0.1f,
							  20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1024, 1024,	// Shadow Width and Height
							  0.1f, 100.0f, // Shadow Near and Far Planes
							  1.0f, 1.0f, 1.0f,
							  2.0f, 2.0f,
							  -0.5f, 3.0f, -9.0f,
							  0.0f, -1.0f, 1.0f,
							  0.9f, 0.2f, 0.1f,
							  20.0f);
	spotLightCount++;
	spotLights[2] = SpotLight(1024, 1024,	// Shadow Width and Height
							  0.1f, 100.0f, // Shadow Near and Far Planes
							  1.0f, 1.0f, 1.0f,
							  2.0f, 2.0f,
							  1.0f, 3.0f, -9.0f,
							  0.0f, -1.0f, 1.0f,
							  0.9f, 0.2f, 0.1f,
							  20.0f);
	spotLightCount++;

	vector<string> skyboxFaces;
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/xpos.png"); // POS X
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/xneg.png"); // NEG X
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/ypos.png"); // POS Y
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/yneg.png"); // NEG Y
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/zpos.png"); // POS Z
	skyboxFaces.push_back("Assets/Textures/Skybox/Interstellar/zneg.png"); // NEG Z
	skybox = SkyBox(skyboxFaces, false, false);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		   uniformSpecularIntensity = 0, uniformShininess = 0;

	// Setting Projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	cerr << "\nLoading complete!\n\n" << endl;

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
		
		if (mainWindow.getsKeys()[GLFW_KEY_KP_7])
		{
		    if (gamma > 0.0f)
		        gamma -= 0.001f;
		    else
		        gamma = 0.0f;

			cout << "GAMMA: " << gamma << "\r";
		}
		else if (mainWindow.getsKeys()[GLFW_KEY_KP_8])
		{
			gamma = 2.2f;
			cout << "GAMMA: " << gamma << "\r";
			mainWindow.getsKeys()[GLFW_KEY_KP_8] = false;
		}
		else if (mainWindow.getsKeys()[GLFW_KEY_KP_9])
		{
		    gamma += 0.001f;
			cout << "GAMMA: " << gamma << "\r";
		}

		// Toggles the TORCH

		DirectionalShadowMapPass(&ambientLight);

		for( size_t i = 0; i < pointLightCount; i++ )
		{
			OmniShadowMapPass(&pointLights[i]);
		}

		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}

		RenderPass(projection, camera.calculateViewMatrix());

		PostProcessingPass();

		// Swap the front and back buffers to display the rendered frame
		mainWindow.swapBuffers();
	}

	// Terminate GLFW and release its resources
	glfwTerminate();

	return 0;
}
