#version 330

in vec4 vColor;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;


const int MAX_POINT_LIGHTS = 16;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;

uniform vec3 eyePosition;

vec4 CalcLightByDirection(Light light, vec3 direction)
{
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;

	// Result of the angle of the light in the object
	// A * B = |A||B|cos(angle)
	// max() so the light on angles too big don't show
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);

	// Calculates only if the surface is hitted by the light
	if( diffuseFactor > 0.0f )
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));

		float specularFactor = dot(fragToEye, reflectedVertex);

		if( specularFactor > 0.0f )
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return (ambientColour + diffuseColour + specularColour);
}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);

	for(int i = 0; i < pointLightCount; i++)
	{
		// Getting the direction
		vec3 direction = FragPos - pointLights[i].position;
		float distance = length(direction);
		direction = normalize(direction);

		vec4 colour = CalcLightByDirection(pointLights[i].base, direction);
		float attenuation = pointLights[i].exponent * distance * distance + pointLights[i].linear * distance + pointLights[i].constant; // AX^2 + BX + C

		totalColour += colour/attenuation;
	}

	return totalColour;
}

void main() 
{
	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLights();

	colour = texture(theTexture, TexCoord0) * finalColour;
	//colour = vColor * finalColour;
}