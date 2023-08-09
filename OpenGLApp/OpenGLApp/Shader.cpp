#include "Shader.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::getline;

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

void Shader::CreateFromFile(const char *vertexLocation, const char *fragmentLocation)
{
    string vertexString = ReadFile(vertexLocation);
    string fragmentString = ReadFile(fragmentLocation);

    // Convert to const char* type of array with c_str
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

string Shader::ReadFile(const char* fileLocation)
{
    string content;
    ifstream fileStream(fileLocation, std::ios::in);

    if( !fileStream.is_open() )
    {
        cerr << "\n\nERROR: Failed to read " << fileLocation << ". \n" << endl;
        return "";
    }

    string line = "";
    while (!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    
    fileStream.close();
    return content;
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

GLuint Shader::GetProjectionLocation()
{
    return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
    return uniformModel;
}

void Shader::UseShader()
{
    glUseProgram(shaderID);
}

// Deleting the program from the GPU
void Shader::ClearShader()
{
    if( shaderID != 0 )
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType)
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

Shader::~Shader()
{
    ClearShader();
}