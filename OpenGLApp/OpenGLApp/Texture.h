#pragma once

#include <iostream>

#include <GL\glew.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>

#include "Config.h"

class Texture
{
public:
    
    Texture();
    Texture(const char* fileLoc);

    bool LoadTexture(bool invertedTexture);
    bool LoadTexture();
    uint32_t LoadTextureID(bool invertedTexture);
    uint32_t LoadTextureIDNormal(bool invertedTexture);
    uint32_t LoadTextureID();
    
    void UseTexture();
    void UseGL_TEXTURE(GLuint id);

    void UsePBRTexture();
    void ClearTexture();

    const char * GetFileLocation() { return fileLocation; }

    ~Texture();


private:

    GLuint textureID;
    int32_t width, height, bitDepth;


    const char* fileLocation;

};

