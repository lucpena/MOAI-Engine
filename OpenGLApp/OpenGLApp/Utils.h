#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <assimp/scene.h>

// Function to translate Texture Type ID to it's Name
inline std::string TextureTypeToString(aiTextureType textureType)
{
    switch (textureType)
    {
    case aiTextureType_NONE:
        return "None";
    case aiTextureType_DIFFUSE:
        return "Diffuse";
    case aiTextureType_SPECULAR:
        return "Specular";
    case aiTextureType_AMBIENT:
        return "Ambient";
    case aiTextureType_EMISSIVE:
        return "Emissive";
    case aiTextureType_HEIGHT:
        return "Height";
    case aiTextureType_NORMALS:
        return "Normals";
    case aiTextureType_SHININESS:
        return "Shininess";
    case aiTextureType_OPACITY:
        return "Opacity";
    case aiTextureType_DISPLACEMENT:
        return "Displacement";
    case aiTextureType_LIGHTMAP:
        return "Lightmap";
    case aiTextureType_REFLECTION:
        return "Reflection";
    case aiTextureType_BASE_COLOR:
        return "Base Color";
    case aiTextureType_NORMAL_CAMERA:
        return "Normal Camera";
    case aiTextureType_EMISSION_COLOR:
        return "Emission Color";
    case aiTextureType_METALNESS:
        return "Metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "Diffuse Roughness";
    case aiTextureType_AMBIENT_OCCLUSION:
        return "Ambient Occlusion";
    case aiTextureType_UNKNOWN:
        return "Unknown";
    default:
        return "Invalid Texture Type";
    }
}

#endif // UTILS_H
