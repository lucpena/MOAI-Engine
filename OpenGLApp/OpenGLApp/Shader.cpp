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

    pointLightCount = 0;
    spotLightCount = 0;
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

void Shader::CreateFromFile(const char *vertexLocation, const char *geometryLocation, const char *fragmentLocation)
{
    string vertexString = ReadFile(vertexLocation);
    string geometryString = ReadFile(geometryLocation);
    string fragmentString = ReadFile(fragmentLocation);

    // Convert to const char* type of array with c_str
    const char *vertexCode = vertexString.c_str();
    const char *fragmentCode = fragmentString.c_str();
    const char *geometryCode = geometryString.c_str();

    CompileShader(vertexCode, geometryCode, fragmentCode);
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

  CompileProgram();
}

void Shader::CompileShader(const char *vertexCode, const char *geometryCode, const char *fragmentCode)
{
  // Create a new shader program and get its ID
  shaderID = glCreateProgram();

  // Check if the shader program was created successfully
  if (!shaderID)
  {
        cerr << "\n\nERROR: Failed to create shader program.\n"
             << endl;
        return;
  }

  // Attach and compile the vertex shader to the shader program
  AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);

  // Attach and compile the fragment shader to the shader program
  AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);

  // Attach and compile the fragment shader to the shader program
  AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

  CompileProgram();
}

// Getting the uniforms
GLuint Shader::GetProjectionLocation() { return uniformProjection; }
GLuint Shader::GetModelLocation() { return uniformModel; }
GLuint Shader::GetViewLocation(){ return uniformView; }
GLuint Shader::GetAmbientColourLocation() { return uniformDirectionalLight.uniformColour; }
GLuint Shader::GetAmbientIntensityLocation() { return uniformDirectionalLight.uniformAmbientIntensity; }
GLuint Shader::GetDiffuseIntensityLocation() { return uniformDirectionalLight.uniformDiffuseIntensity; }
GLuint Shader::GetDirectionLocation() { return uniformDirectionalLight.uniformDirection; }
GLuint Shader::GetSpecularIntensityLocation(){ return uniformSpecularIntensity; }
GLuint Shader::GetShininessLocation() { return uniformShininess; }
GLuint Shader::GetOmniLightPosLocation() { return uniformOmniLightPos; }
GLuint Shader::GetFarPlaneLocation() { return uniformFarPlane; }
GLuint Shader::GetEyePositionLocation() { return uniformEyePosition; }

void Shader::SetDirectionalLight(DirectionalLight *dLight)
{
    dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColour,
                     uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight *pLight, uint32_t lightCount, uint32_t textureUnit, uint32_t offset)
{   
    if( lightCount > MAX_POINT_LIGHTS ) lightCount = MAX_POINT_LIGHTS;

    glUniform1i(uniformPointLightCount, lightCount);

    for( size_t i = 0; i < lightCount; i++ )
    {
        pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity,
                           uniformPointLight[i].uniformColour,
                           uniformPointLight[i].uniformDiffuseIntensity,
                           uniformPointLight[i].uniformPosition,
                           uniformPointLight[i].uniformConstant,
                           uniformPointLight[i].uniformLinear,
                           uniformPointLight[i].uniformExponent);

        pLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);

        // Attaching the shadow maps
        glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);

        // Farplane
        glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, pLight[i].GetFarPlane());
    }

}

void Shader::SetSpotLights(SpotLight *sLight, uint32_t lightCount, uint32_t textureUnit, uint32_t offset)
{
    if( lightCount > MAX_SPOT_LIGHTS ) lightCount = MAX_SPOT_LIGHTS;

    glUniform1i(uniformSpotLightCount, lightCount);

    for( size_t i = 0; i < lightCount; i++ )
    {
        sLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity,
                           uniformSpotLight[i].uniformColour,
                           uniformSpotLight[i].uniformDiffuseIntensity,
                           uniformSpotLight[i].uniformPosition,
                           uniformSpotLight[i].uniformDirection,
                           uniformSpotLight[i].uniformConstant,
                           uniformSpotLight[i].uniformLinear,
                           uniformSpotLight[i].uniformExponent,
                           uniformSpotLight[i].uniformEdge);

        sLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);

        // Attaching the shadow maps
        glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);

        // Farplane
        glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, sLight[i].GetFarPlane());
    }
}

void Shader::SetTexture(GLuint textureUnit)
{
    glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetNormalMap(GLuint textureUnit)
{
    glUniform1i(uniformNormalMap, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
    glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4 *lTransform)
{
    glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Shader::SetLightMatrices(vector<glm::mat4> lightMatrices)
{
    for( size_t matrix = 0; matrix < 6; matrix++ )
    {
        glUniformMatrix4fv(uniformLightMatrices[matrix], 1, GL_FALSE, glm::value_ptr(lightMatrices[matrix]));
    }
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(shaderID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(shaderID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shaderID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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

void Shader::Validate()
{
    GLint result = 0;
    GLchar eLog[1024] = {0};

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
}

void Shader::CompileProgram()
{
    GLint result = 0;
    GLchar eLog[1024] = {0};

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

    // Shader compilation and linking successful
    // The shader program is now ready for use in rendering.

    // Getting the values from the Shaders
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

    // Getting the values from the Point Lights
    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        // \0 Grants that's it'll be aways a string
        char locBuffer[100] = {'\0'};

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].base.colour", i);
        uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "pointLights[%zd].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);
    }

    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

    // Getting the values from the Spot Lights
    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        // \0 Grants that's it'll be aways a string
        char locBuffer[100] = {'\0'};

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.base.colour", i);
        uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.base.ambientIntensity", i);
        uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.base.diffuseIntensity", i);
        uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.position", i);
        uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.constant", i);
        uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.linear", i);
        uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].base.exponent", i);
        uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].direction", i);
        uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "spotLights[%zd].edge", i);
        uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuffer);
    }

    uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
    uniformTexture = glGetUniformLocation(shaderID, "theTexture");
    uniformNormalMap = glGetUniformLocation(shaderID, "normalMapTexture");
    uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

    uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
    uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");

    for (size_t matrixIndex = 0; matrixIndex < 6; matrixIndex++)
    {
        // \0 Grants that's it'll be aways a string
        char locBuffer[100] = {'\0'};

        snprintf(locBuffer, sizeof(locBuffer), "lightMatrices[%zd]", matrixIndex);
        uniformLightMatrices[matrixIndex] = glGetUniformLocation(shaderID, locBuffer);
    }

    for (size_t matrixIndex = 0; matrixIndex < MAX_LIGHTS; matrixIndex++)
    {
        // \0 Grants that's it'll be aways a string
        char locBuffer[100] = {'\0'};

        snprintf(locBuffer, sizeof(locBuffer), "omniShadowMaps[%zd].shadowMap", matrixIndex);
        uniformOmniShadowMap[matrixIndex].uniformShadowMap = glGetUniformLocation(shaderID, locBuffer);

        snprintf(locBuffer, sizeof(locBuffer), "omniShadowMaps[%zd].farPlane", matrixIndex);
        uniformOmniShadowMap[matrixIndex].uniformFarPlane = glGetUniformLocation(shaderID, locBuffer);
    }
}

Shader::~Shader()
{
    ClearShader();
}