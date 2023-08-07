#include "Shader.h"

using std::cerr;
using std::cout;
using std::endl;

Shader::Shader()
{
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::CreateFromString(const char *vertexCode, const char *fragmentCode)
{
    CompileShader(vertexCode, fragmentCode);
}

void Shader::CompileShader(const char *vertexCode, const char *fragmentCode)
{
// Create a new shader program and get its ID
	shaderID = glCreateProgram();

	// Check if the shader program was created successfully
    if (!shaderID)
    {
		cerr << "\n\nERROR: Failed to create shader program.\n" << endl;
		return;
	}

	// Attach and compile the vertex shader to the shader program
    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);

    // Attach and compile the fragment shader to the shader program
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Link the shader program to create the final executable in the GPU
    glLinkProgram(shaderID);

    // Check if the linking was successful
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

    if (!result)
	{
		// If linking failed, get the error log and display it
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        cerr << "\n\nERROR: Failed to link program: " << eLog << endl;
		return;
	}

	// Validate the shader program to check for potential issues
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

    if (!result)
	{
		// If validation failed, get the error log and display it
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        cerr << "\n\nERROR: Failed to validate program: " << eLog << endl;
		return;
	}

	// Shader compilation and linking successful
	// The shader program is now ready for use in rendering.

    // Getting the variables from the Shaders
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection"); // Create a new shader program and get its ID
   
}