#version 330

layout (location = 0) in vec3 position;

uniform mat4 model; // Converts the position of the LIGHT to WORLD Space
uniform mat4 directionalLightTransform;    // Projection * View

void main()
{
    gl_Position = directionalLightTransform * model * vec4(position, 1.0);
}