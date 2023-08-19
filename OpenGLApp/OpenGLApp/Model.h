#pragma once

#include <iostream>
#include <vector>
#include <string>

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

class Model
{
public:
    Model();

    void LoadModel(const string& fileName, const string& objName, bool invertedTexture);
    void LoadModel(const string &fileName, const string &objName);
    void RenderModel();
    void ClearModel();

    ~Model();

private:

    void LoadNode(aiNode* node, const aiScene* scene);
    void LoadMesh(aiMesh* mesh, const aiScene* scene);
    void LoadMaterials(const aiScene* scene, const string &objName, bool invertedTexture);

    vector<Mesh*> meshList;
    vector<Texture*> textureList;
    vector<uint32_t> meshToTex;

};

