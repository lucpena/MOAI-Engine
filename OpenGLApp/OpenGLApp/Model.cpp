#include "Model.h"

Model::Model()
{
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
    for ( size_t i = 0; i < meshList.size(); i++ )
    {
        uint32_t materialIndex = meshToTex[i];

        if( materialIndex < textureList.size() && textureList[materialIndex])
        {
            textureList[materialIndex]->UseTexture();
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
    textureList.resize(scene->mNumMaterials);

    for( size_t i = 0; i < scene->mNumMaterials; i++ )
    {
        aiMaterial* material = scene->mMaterials[i];
        
        textureList[i] = nullptr;

        if( material->GetTextureCount(aiTextureType_DIFFUSE) )
        {
            aiString path;
            if( material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS )
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                textureList[i] = new Texture(texPath.c_str());

                cerr << "Loading DIFFUSE " << fileExtension << " texture: " << fileName << " [" << i << " of " << scene->mNumMaterials - 1 << "]..." << endl;
                if (!textureList[i]->LoadTexture(invertedTexture))
                {
                    cerr << "\n\nERROR: Failed to load DIFFUSE Texture " << fileName << ".\n"
                         << endl;
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR))
        {
            aiString path;
            if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                textureList[i] = new Texture(texPath.c_str());

                cerr << "Loading SPECULAR " << fileExtension << " texture: " << fileName << " [" << i << " of " << scene->mNumMaterials - 1 << "]..." << endl;
                if (!textureList[i]->LoadTexture(invertedTexture))
                {
                    cerr << "\n\nERROR: Failed to load DIFFUSE Texture " << fileName << ".\n"
                         << endl;
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        if (material->GetTextureCount(aiTextureType_HEIGHT)) // NORMAL
        {
            aiString path;
            if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                textureList[i] = new Texture(texPath.c_str());

                cerr << "Loading HEIGHT(NORMAL) " << fileExtension << " texture: " << fileName << " [" << i << " of " << scene->mNumMaterials - 1 << "]..." << endl;
                if (!textureList[i]->LoadTexture(invertedTexture))
                {
                    cerr << "\n\nERROR: Failed to load DIFFUSE Texture " << fileName << ".\n"
                         << endl;
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        if (material->GetTextureCount(aiTextureType_AMBIENT)) // HEIGHT MAP
        {
            aiString path;
            if (material->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
            {
                // Fixes the path if it is relative
                int32_t idx = string(path.data).rfind("\\");
                string fileName = string(path.data).substr(idx + 1);

                string texPath = string("Assets/Models/Textures/") + objName + string("/") + fileName;

                // Gettin the file extension
                int32_t extIndex = string(fileName).rfind(".");
                string fileExtension = string(fileName).substr(extIndex + 1);

                textureList[i] = new Texture(texPath.c_str());

                cerr << "Loading AMBIENT " << fileExtension << " texture: " << fileName << " [" << i << " of " << scene->mNumMaterials - 1 << "]..." << endl;
                if (!textureList[i]->LoadTexture(invertedTexture))
                {
                    cerr << "\n\nERROR: Failed to load DIFFUSE Texture " << fileName << ".\n"
                         << endl;
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        // Sets the texture to a plain white if not found
        if( !textureList[i] )
        {
            textureList[i] = new Texture("Assets/Textures/plain.png");
            textureList[i]->LoadTexture();
        }
    }
}

Model::~Model()
{
}