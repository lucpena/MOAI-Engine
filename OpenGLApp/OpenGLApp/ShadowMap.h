#pragma once

#include<iostream>

#include <GL\glew.h>

using std::cerr;
using std::cout;
using std::endl;

class ShadowMap
{
public:

    ShadowMap();

    virtual bool Init(uint32_t width, uint32_t height);
    virtual void Write();
    virtual void Read(GLenum textureUnit);

    GLuint GetShadowWidth();
    GLuint GetShadowHeight();

    ~ShadowMap();

private:

    GLuint FBO, shadowMap;
    GLuint shadowWidth, shadowHeight;
};

