#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
    FBO = 0;
    shadowMap = 0;
}

bool ShadowMap::Init(uint32_t width, uint32_t height)
{
    shadowWidth = width;
    shadowHeight = height;

    // Creates a framebuffer for the Shadows
    glGenFramebuffers(1, &FBO);

    // Creates the Texture
    glGenTextures(1, &shadowMap);

    // Bind the texture to the target
    glBindTexture(GL_TEXTURE_2D, shadowMap);

    // Texture that will receive the output of the FBO
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Parameters of the texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Binding the FBO to the Framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    // Connect the framebuffer to the texture
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    // Dont read the color attachment
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check for errors
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        cerr << "\nERROR: Framebuffer Error. " << status << ".\n" << endl;
        return false;
    }

    return true;
}

void ShadowMap::Write()
{
    // Binding the FBO to the Framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void ShadowMap::Read(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}


GLuint ShadowMap::GetShadowWidth() { return shadowWidth; }
GLuint ShadowMap::GetShadowHeight() { return shadowHeight; }

ShadowMap::~ShadowMap()
{
    if( FBO )
    {
        glDeleteFramebuffers(1, &FBO);
    }

    if( shadowMap )
    {
        glDeleteTextures(1, &shadowMap);
    }
}
