#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];  // One for each face of the cube map

out vec4 FragPos;

void main()
{
    for( int face = 0; face < 6; face++ )
    {
        gl_Layer = face;

        for( int vertices = 0; vertices < 3; vertices++ )
        {
            // gl_in gets the layout triangles in
            FragPos = gl_in[vertices].gl_Position;

            // lightmatrix is a combination of the view and projection matrices of the light source
            gl_Position = lightMatrices[face] * FragPos;

            EmitVertex();
        }

        // Finished the triangle strip
        EndPrimitive();
    }
}