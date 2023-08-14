#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include "Config.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

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
    

    void SetDirectionalLight(DirectionalLight* dLight);
    void SetPointLights(PointLight* pLight, uint32_t lightCount);
    void SetSpotLights(SpotLight* sLight, uint32_t lightCount);

    void UseShader();
    void ClearShader();

    ~Shader();

private:

    GLuint shaderID, uniformProjection, uniformModel, uniformView, 
    uniformEyePosition, uniformSpecularIntensity, uniformShininess;

    uint32_t pointLightCount;
    uint32_t spotLightCount;

    struct
    {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformDirection;
    } uniformDirectionalLight;

    GLuint uniformPointLightCount;

    struct
    {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformExponent;
        GLuint uniformLinear;
    } uniformPointLight[MAX_POINT_LIGHTS];

    GLuint uniformSpotLightCount;

    struct
    {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformExponent;
        GLuint uniformLinear;

        GLuint uniformDirection;
        GLuint uniformEdge;
    } uniformSpotLight[MAX_SPOT_LIGHTS];

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
};

