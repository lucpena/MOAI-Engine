#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Config.h"
#include "Shader.h"
#include "Mesh.h"

using std::vector;
using std::string;

class SkyBox
{
public:

    SkyBox();
    SkyBox(vector<string> faceLocations, bool invertedTexture, bool hasAlpha);

    void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);


    ~SkyBox();

private:

    Mesh* skyMesh;
    Shader* skyShader;

    GLuint textureID;
    GLuint uniformProjection, uniformView;

};

