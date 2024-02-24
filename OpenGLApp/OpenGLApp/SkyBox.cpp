#include "SkyBox.h"

SkyBox::SkyBox()
{
}

SkyBox::SkyBox(vector<string> faceLocations, bool invertedTexture, bool hasAlpha)
{
    // Shader setup
    skyShader = new Shader();
    skyShader->CreateFromFile("Shaders/skybox.vert", "Shaders/skybox.frag");

    uniformProjection = skyShader->GetProjectionLocation();
    uniformView = skyShader->GetViewLocation();

    // Texture setup
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // bool invertedTexture = false;
    // bool hasAlpha = false;

    int width, height, bitDepth;

    if (invertedTexture)
        stbi_set_flip_vertically_on_load(true);

    cerr << endl;
    cerr << "Loading Skybox..." << endl;
    cerr << "----------------------------------" << endl;

    for (size_t i = 0; i < 6; i++)
    {
        cerr << "Loading Skybox texture: " << faceLocations[i].c_str() << " [" << i + 1 << " of " << 6 << "]..." << endl;

        unsigned char *texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);

        if (!texData)
        {
            cerr << "\n\nERROR: Failed to find the Skybox texture " << faceLocations[i].c_str() << ".\n" << endl;
            // return;
        }

        if( !hasAlpha )
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

        if( hasAlpha )
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

        stbi_image_free(texData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Away from the image || or GL_NEAREST
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Closer to the image || or GL_NEAREST

    // Mesh Setup Code (to save you havin to type out the vertices of a cube!)
    unsigned int skyboxIndices[] = {
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 5,
        5, 3, 7,
        // back
        5, 7, 4,
        4, 7, 6,
        // left
        4, 6, 0,
        0, 6, 1,
        // top
        4, 0, 5,
        5, 0, 2,
        // bottom
        1, 6, 3,
        3, 6, 7};

    float skyboxVertices[] = {
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    skyMesh = new Mesh();
    skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
}

void SkyBox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    // Cut the transform data from the original matrix. Keeps at the origin
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));

    // glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    skyShader->UseShader();

    // Setting the Projection Matrix
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Setting the View Matrix
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    skyShader->Validate();

    skyMesh->RenderMesh();

    // glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

SkyBox::~SkyBox()
{

}
