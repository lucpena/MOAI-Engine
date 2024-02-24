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
    void SetNormalMap(GLuint textureUnit);
    void SetDirectionalShadowMap(GLuint textureUnit);
    void SetDirectionalLightTransform(glm::mat4* lTransform);
    void SetLightMatrices(vector<glm::mat4> lightMatrices);

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void UseShader();
    void ClearShader();

    ~Shader();

private:

    GLuint shaderID, uniformProjection, uniformModel, uniformView, 
    uniformEyePosition, uniformSpecularIntensity, uniformShininess,
    uniformDirectionalLightTransform, uniformDirectionalShadowMap,
    uniformTexture, uniformOmniLightPos, uniformFarPlane, uniformNormalMap;

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

