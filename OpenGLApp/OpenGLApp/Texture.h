#pragma once

#include <iostream>

#include <GL\glew.h>

#include "Config.h"

class Texture
{
public:
    
    Texture();
    Texture(const char* fileLoc);

    bool LoadTexture(bool invertedTexture);
    bool LoadTexture();
    void UseTexture();
    void ClearTexture();

    ~Texture();


private:

    GLuint textureID;
    int32_t width, height, bitDepth;


    const char* fileLocation;

};

