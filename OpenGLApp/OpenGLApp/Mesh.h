#pragma once

#include <iostream>
#include <GL\glew.h>

class Mesh
{
public:

    Mesh();
    void CreateMesh( GLfloat* vertices, uint32_t* indices, uint32_t numOfVertices, uint32_t numOfIndices );
    void RenderMesh();
    void ClearMesh();

    ~Mesh();

private:

    GLuint VAO, VBO, IBO;
    GLsizei indexCount; 

};