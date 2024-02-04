#pragma once

#include <iostream>

#include <GL\glew.h>

using std::cerr;
using std::cout;
using std::endl;

class HDR
{
public:
    HDR();

    virtual bool Init(uint32_t width, uint32_t height);
    virtual void Write();
    virtual void Read(GLenum textureUnit);

    ~HDR();

protected:
    GLuint FBO, shadowMap;
    GLuint shadowWidth, shadowHeight;
};
