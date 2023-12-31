#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;
layout (location = 2) in vec3 norm;

out vec4 vColor;
out vec2 TexCoord0;
out vec3 Normal;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform mat4 directionalLightTransform;    // Projection * View


void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	DirectionalLightSpacePos = directionalLightTransform * model * vec4(position, 1.0);

	vColor = vec4(clamp(position, 0.0f, 1.0f), 1.0f);

	TexCoord0 = texture;

	Normal = mat3(transpose(inverse( model ))) * norm;

	FragPos = (model * vec4(position, 1.0)).xyz;
}