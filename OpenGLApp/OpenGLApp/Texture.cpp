#include "Texture.h"

using std::cerr;
using std::cout;
using std::endl;

Texture::Texture()
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = "";
}

Texture::Texture(char* _fileLocation)
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = _fileLocation;
}

void Texture::LoadTexture()
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

    if( !texData )
    {
        cerr << "\n\nERROR: Failed to find the image " << fileLocation << ".\n" << endl;
    }

    // Generating the texture and giving it an ID
    glGenTextures(1, &textureID);

    // Binding
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setting parameters for this texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Away from the image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Closer to the image

    // Setting the Texture
    // Can be a problem with the texture. Make shure it's the right one here <GL_RGBA or GL_RGB>
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

    // Create mipmaps automatically
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture loaded to the memory! UwU

    // Goodbye texture 
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texData);
}

void Texture::UseTexture()
{
    // GL_TEXTURE0 is a Texture unit
    glActiveTexture(GL_TEXTURE0);

    // Binding the Texture to the Texture Unit
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = "";
}

Texture::~Texture()
{
    ClearTexture();
}