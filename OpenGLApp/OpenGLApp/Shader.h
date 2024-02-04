#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

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
    void CreateFromFile(const char *vertexLocation, const char *geometryLocation, const char *fragmentLocation);

    void Validate();

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
    GLuint GetOmniLightPosLocation();
    GLuint GetFarPlaneLocation();
    
    void SetDirectionalLight(DirectionalLight* dLight);
    void SetPointLights(PointLight *pLight, uint32_t lightCount, uint32_t textureUnit, uint32_t offset);
    void SetSpotLights(SpotLight *sLight, uint32_t lightCount, uint32_t textureUnit, uint32_t offset);
    void SetTexture(GLuint textureUnit);
    void SetDirectionalShadowMap(GLuint textureUnit);
    void SetDirectionalLightTransform(glm::mat4* lTransform);
    void SetLightMatrices(vector<glm::mat4> lightMatrices);

    void SetInt(const string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;

    void UseShader();
    void ClearShader();

    ~Shader();

private:

    GLuint shaderID, uniformProjection, uniformModel, uniformView, 
    uniformEyePosition, uniformSpecularIntensity, uniformShininess,
    uniformDirectionalLightTransform, uniformDirectionalShadowMap,
    uniformTexture, uniformOmniLightPos, uniformFarPlane;

    GLuint uniformLightMatrices[6];

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


    struct 
    {
        GLuint uniformShadowMap;
        GLuint uniformFarPlane;
    } uniformOmniShadowMap[MAX_LIGHTS];

    void CompileShader(const char *vertexCode, const char *fragmentCode);
    void CompileShader(const char *vertexCode, const char *geometryLocation, const char *fragmentCode);
    void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);

    void CompileProgram();
};

