#include "Mesh.h"

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices, uint32_t *indices, uint32_t numOfVertices, uint32_t numOfIndices)
{
    indexCount = numOfVertices;

    // Allocates GPU memory space for 1 Vertex Array object (VAO)
    glGenVertexArrays(1, &VAO);

    // Bind the newly created VAO, all subsequent operations will be performed on this VAO
    glBindVertexArray(VAO);

    // Creates a Index Buffer Object (IBO) inside the VAO to store the indices data
    glGenBuffers(1, &IBO);

    // Bind the IBO to the GL_ELEMENT_ARRAY_BUFFER target
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Transfer the indices data to the GPU memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    // Creates a Vertex Buffer Object (VBO) inside the VAO to store the vertex data
    glGenBuffers(1, &VBO);

    // Bind the VBO to the GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Transfer the vertex data to the GPU memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    // Define the layout of the vertex data within the VBO
    // Attribute 0 (index 0) corresponds to the position attribute of the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable the vertex attribute at index 0 (position attribute)
    glEnableVertexAttribArray(0);

    // Unbind the VBO (not necessary, but good practice to unbind when done)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO (not necessary, but good practice to unbind when done)
    glBindVertexArray(0);

    // Unbind the IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
    // Bind the vertex array object containing the triangle data
    glBindVertexArray(VAO);

    //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Draw the triangle using OpenGL draw call
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    // Unbind the IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Unbind the vertex array object
    glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
    if( IBO != 0 )
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}

Mesh::~Mesh()
{
    ClearMesh();
}