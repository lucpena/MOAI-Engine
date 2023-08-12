#version 330

in vec4 vColor;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePosition;

void main() 
{
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

	// Result of the angle of the light in the object
	// A * B = |A||B|cos(angle)
	// max() so the light on angles too big don't show
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);

	// Calculates only if the surface is hitted by the light
	if( diffuseFactor > 0.0f )
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(directionalLight.direction, normalize(Normal)));

		float specularFactor = dot(fragToEye, reflectedVertex);

		if( specularFactor > 0.0f )
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(directionalLight.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	//colour = texture(theTexture, TexCoord0) * (ambientColour + diffuseColour + specularColour);
	colour = vColor* (ambientColour + diffuseColour + specularColour);
}