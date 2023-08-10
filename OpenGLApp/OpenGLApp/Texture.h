#pragma once

#include <iostream>

#include <GL\glew.h>

#include "stb_image.h"

class Texture
{
public:
    
    Texture();
    Texture(char* fileLoc);

    void LoadTexture();
    void UseTexture();
    void ClearTexture();

    ~Texture();


private:

    GLuint textureID;
    int32_t width, height, bitDepth;

    char* fileLocation;

};

