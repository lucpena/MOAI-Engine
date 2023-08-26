#pragma once
#include "ShadowMap.h"


class OmniShadowMap :
    public ShadowMap
{
public:

    OmniShadowMap();

    bool Init(uint32_t width, uint32_t height);
    void Write();
    void Read(GLenum textureUnit);

    ~OmniShadowMap();
};

