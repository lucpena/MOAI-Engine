#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

using std::string;

class Shader
{
public:

    Shader();

    void CreateFromString(const char *vertexCode, const char *fragmentCode);
    void CreateFromFile(const char *vertexLocation, const char *fragmentLocation);

    string ReadFile(const char* fileLocation);

    GLuint GetProjectionLocation();
    GLuint GetModelLocation();
    GLuint GetViewLocation();
    GLuint GetAmbientIntensityLocation();
    GLuint GetAmbientColourLocation();
    GLuint GetDiffuseIntensityLocation();
    GLuint GetDirectionLocation();
    GLuint GetEyePositionLocation();
    GLuint GetSpecularIntensityLocation();
    GLuint GetShininessLocation();

    void UseShader();
    void ClearShader();

    ~Shader();

private:

    GLuint shaderID, uniformProjection, uniformModel, uniformView, 
    uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity,
    uniformDirection, uniformEyePosition, uniformSpecularIntensity,
    uniformShininess;

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
};

