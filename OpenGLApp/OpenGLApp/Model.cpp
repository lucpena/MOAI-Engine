#include "Model.h"
#include "Utils.h"

Model::Model()
{
    albedoMap = -1;
    normalMap = -1;
    metallicMap = -1;
    roughnessMap = -1;
    AOMap = -1;
}

void Model::LoadModel(const string& fileName, const string& objName)
{
    cerr << "\nLoading model [" << objName << "]..." << endl;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene)
    {
        cerr << "\n\nERROR: Failed to load ASSIMP Scene at " << fileName << ". " << importer.GetErrorString() << ".\n" << endl;
        return;
    }

    LoadNode(scene->mRootNode, scene);
    LoadMaterials(scene, objName, false);
}

void Model::LoadModel(const string &fileName, const string &objName, bool invertedTexture)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
    // const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate );

    if( !scene )
    {
        cerr << "\n\nERROR: Failed to load ASSIMP Scene at " << fileName << ". " << importer.GetErrorString() << ".\n" << endl;
        return;
    }

    LoadNode(scene->mRootNode, scene);
    LoadMaterials(scene, objName, invertedTexture);
}

void Model::RenderModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        uint32_t materialIndex = meshToTex[i];

        if (materialIndex < textureList.size() && textureList[materialIndex])
        {
            textureList[materialIndex]->UseTexture();
        }

        if (normalMap != -1 && normalList[materialIndex])
        {
            normalList[materialIndex]->UseGL_TEXTURE(3);
            // glActiveTexture(GL_TEXTURE3);
            // glBindTexture(GL_TEXTURE_2D, normalMap);
            //glUniform1i(normalMap, 2); // O Normal Map está na unidade de textura 1
        }
        meshList[i]->RenderMesh();
    }
}

void Model::ClearModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        if (meshList[i])
        {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }

    for (size_t i = 0; i < textureList.size(); i++)
    {
       if (textureList[i])
       {
           delete textureList[i];
           textureList[i] = nullptr;
       }
    }
}


void Model::LoadNode(aiNode* node, const aiScene* scene)
{
    for( size_t i = 0; i < node->mNumMeshes; i++ )
    {
        LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for( size_t i = 0; i < node->mNumChildren; i++ )
    {
        LoadNode(node->mChildren[i], scene);
    }
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<GLfloat> vertices;
    vector<uint32_t> indices;

    for( size_t i = 0; i < mesh->mNumVertices; i++ )
    {
        // Adds the vertices coordinates 
        vertices.insert(vertices.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});

        // Check if the Texture exists
        if( mesh->mTextureCoords[0] )
        {
            vertices.insert(vertices.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
        } else 
        {
            vertices.insert(vertices.end(), {0.0f, 0.0f});    
        }

        // Gets the Normals
        // Minus because they are not negative in the FRAG SHADER as normaly are
        vertices.insert(vertices.end(), {-mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z});
    }

    // Getting the indices
    for( size_t i = 0; i < mesh->mNumFaces; i++ )
    {
        aiFace face = mesh->mFaces[i];
        for( size_t j = 0; j < face.mNumIndices; j++ )
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh( &vertices[0], &indices[0], vertices.size(), indices.size() );
    meshList.push_back(newMesh);
    meshToTex.push_back(mesh->mMaterialIndex);
}



void Model::LoadMaterials(const aiScene* scene, const string &objName, bool invertedTexture)
{
    cerr << "----------------------------------" << endl;

    //textureList.resize(scene->mNumMaterials);
    //textureList.resize(aiTextureType_UNKNOWN);

    textureList.resize(scene->mNumMaterials);
    normalList.resize(scene->mNumMaterials);

    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial *material = scene->mMaterials[i];

        textureList[i] = nullptr;
        normalList[i] = nullptr;

        if (material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                textureList[i] = new Texture(texPath.c_str());
                // Texture *newTexture = new Texture(texPath.c_str());
                // textureList.push_back(newTexture);

                cerr << "Loading DIFFUSE " << fileExtension << " texture: " << fileName << " [" << i << " of " << scene->mNumMaterials - 1 << "]..." << endl;

                albedoMap = textureList[i]->LoadTextureID(invertedTexture);
                // albedoMap = newTexture->LoadTextureID(invertedTexture);

                if (albedoMap == -1)
                {
                    cerr << "\n\nERROR: Failed to load DIFFUSE Texture " << fileName << ".\n"
                         << endl;
                    delete textureList[i];
                    textureList[i] = nullptr;
                } 

            }
        }

        if (material->GetTextureCount(aiTextureType_NORMALS)) // NORMAL
        {
            aiString path;
            if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                normalList[i] = new Texture(texPath.c_str());
                // Texture *newTexture = new Texture(texPath.c_str());
                // normalList.push_back(newTexture);

                cerr << "Loading HEIGHT(NORMAL) " << fileExtension << " texture: " << fileName << " [" << aiTextureType_NORMALS << " of " << scene->mNumMaterials - 1 << "]..." << endl;

                normalMap = normalList[i]->LoadTextureID(invertedTexture);
                // normalMap = newTexture->LoadTextureID(invertedTexture);

                if (normalMap == -1)
                {
                    cerr << "\n\nERROR: Failed to load NORMAL Texture " << fileName << ".\n"
                         << endl;
                    delete normalList[aiTextureType_NORMALS];
                    normalList[aiTextureType_NORMALS] = nullptr;
                } 
            }
        }     

        // Sets the texture to a plain white if not found
        // if (!textureList[i])
        // {
        //     textureList[i] = new Texture("Assets/Textures/plain.png");
        //     textureList[i]->LoadTexture();
        // }
    }

    // cerr << "\tAlbedo map loc: " << albedoMap << endl
    //      << "\tNormal map loc: " << normalMap << endl
    //      << "\tMetaless map loc: " << metallicMap << endl
    //      << "\tRoughness map loc: " << roughnessMap << endl
    //      << "\tAO map loc: " << AOMap << endl;
         
}

Model::~Model()
{
}