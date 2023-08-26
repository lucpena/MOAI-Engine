#version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;     // Distance to render, like the FAR from the camera


void main()
{
    float distance = length(FragPos.xyz - lightPos);
    distance = distance/farPlane;

    // Depth check for the Depth Attachment
    gl_FragDepth = distance;
}