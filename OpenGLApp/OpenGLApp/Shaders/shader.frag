#version 330

in vec4 vColor;
in vec2 TexCoord0;
in vec3 Normal;

out vec4 colour;

struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main() 
{
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

	// Result of the angle of the light in the object
	// A * B = |A||B|cos(angle)
	// max() so the light on angles too big don't show
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	colour = texture(theTexture, TexCoord0) * (ambientColour + diffuseColour);
	//colour = texture(theTexture, TexCoord0) * vColor;
}