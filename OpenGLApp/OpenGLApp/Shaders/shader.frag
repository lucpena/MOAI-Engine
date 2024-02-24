#version 330 core

in vec4 vColor;
in vec2 TexCoord0;
in vec2 NormalMap;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 colour;


const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS  = 16;

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

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

uniform float gamma;


vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float CalcDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; // Getting the coordinate system (w is the Forth value)
	projCoords = (projCoords * 0.5) + 0.5;	// Values between 0 and 1
	
	float current = projCoords.z;// How far it is from the light
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(directionalLight.direction);
	
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);

	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}									
	
	return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
	vec3 fragToLight = FragPos - light.position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.05;
	int samples = 20;

	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance/omniShadowMaps[shadowIndex].farPlane)) / 25.0;

	for( int i = 0; i < samples; i++ )
	{
		float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= omniShadowMaps[shadowIndex].farPlane;

		if(currentDepth - bias > closestDepth)
		{
			shadow += 1.0;
		}
	}

	shadow /= float(samples);
	return shadow * 2.8;
	//return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;

	// Result of the angle of the light in the object
	// A * B = |A||B|cos(angle)
	// max() so the light on angles too big don't show
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);

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

	return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour));
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
	vec3 direction = FragPos - pLight.position;
	float distance = length(direction);
	direction = normalize(direction);

	float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);

	vec4 colour = CalcLightByDirection(pLight.base, direction, shadowFactor);
	// float attenuation = pLight.exponent * distance * distance + pLight.linear * distance + pLight.constant; // AX^2 + BX + C
	
	float attenuation = pLight.exponent * distance * distance + pLight.linear * distance + pLight.constant; // AX^2 + BX + C
	attenuation = pow(attenuation, 1.0/2.2);

	return colour/attenuation;
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if( slFactor > sLight.edge )
	{
		vec4 colour = CalcPointLight(sLight.base, shadowIndex);

		return colour * (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge)));

	} else
	{
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalcPointLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);

	for(int i = 0; i < pointLightCount; i++)
	{
		totalColour += CalcPointLight(pointLights[i], i);
	}

	return totalColour;
}

vec4 CalcSpotLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);

	for(int i = 0; i < spotLightCount; i++)
	{
		totalColour += CalcSpotLight(spotLights[i], i + pointLightCount);
	}

	return totalColour;
}

void main() 
{
	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLights();
	finalColour += CalcSpotLights();

	//HDR tone mapping
	finalColour = finalColour / (finalColour + vec4(1.0));

	// Gamma correction
    finalColour.rgb = pow(finalColour.rgb, vec3(1.0 / gamma));

	colour = texture(theTexture, TexCoord0) * finalColour;
}