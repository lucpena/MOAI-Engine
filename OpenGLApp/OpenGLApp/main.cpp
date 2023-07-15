#include <iostream>

//#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

using std::cout;
using std::endl;
using std::cerr;

// Pressing a button
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mode )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{
		glfwSetWindowShouldClose( window, GL_TRUE );
	}
}

int main() {

	glfwInit();

	// Initializing the OpenGL version 3.3
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	GLFWwindow* window = glfwCreateWindow( 800, 600, "MOAI Engine", nullptr, nullptr );
	
	glfwSetKeyCallback( window, key_callback );


	if( window == nullptr )
	{
		cerr << "\n\nERROR: Failed to create GLFW Window.\n\n" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent( window );

	// Initializating GLEW
	glewExperimental = GL_TRUE;

	if( glewInit() != GLEW_OK )
	{
		cerr << "\n\nERROR: Failed to initialize GLEW.\n\n" << endl;
		return -1;
	}

	glViewport( 0, 0, 800, 600 );

	while ( !glfwWindowShouldClose(window) )
	{
		glfwPollEvents();

		glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glfwSwapBuffers( window );
	}

	glfwTerminate();	

	return 0;
}