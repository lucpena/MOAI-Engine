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
    
    void UseShader();
    void ClearShader();

    ~Shader();

private:

    GLuint shaderID, uniformProjection, uniformModel;

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
};

