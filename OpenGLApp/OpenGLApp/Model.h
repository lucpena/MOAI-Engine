#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "Texture.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using MaterialTextureMap = std::unordered_map<uint32_t, vector<Texture *>>;

class Model
{
public:
    Model();

    void LoadModel(const string& fileName, const string& objName, bool invertedTexture);
    void LoadModel(const string &fileName, const string &objName);
    void RenderModel();
    void ClearModel();

    ~Model();

    int32_t albedoMap, normalMap, metallicMap, roughnessMap, AOMap;

private:

    void LoadNode(aiNode* node, const aiScene* scene);
    void LoadMesh(aiMesh* mesh, const aiScene* scene);
    void LoadMaterials(const aiScene* scene, const string &objName, bool invertedTexture);
    void LoadTextureOfType(aiMaterial *material, aiTextureType type, const string &objName, bool invertedTexture);
    MaterialTextureMap materialTexturesMap;

    vector<Mesh*>       meshList;
    vector<Texture*>    textureList;
    vector<Texture*>    normalList;
    vector<GLuint>      texType;        // Type of texture for PBR
    vector<uint32_t>    meshToTex;
};

