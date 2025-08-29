#version 450

layout(binding = 1) uniform UniformLightObject {
    int type;
	float intensity;
	float range;
	float angle;
    vec3 direction;
    vec3 position;
	vec3 color;
} light;

layout(binding = 2) uniform sampler2D glossMap;
layout(binding = 3) uniform sampler2D specMap;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec3 cameraPosition;
layout(location = 6) in vec3 worldPosition;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

vec4 CalculateSpecular(vec3 normal, vec3 viewDirection);

float minOA = 0.02;
float maxOA = 1;

void main()
{
    

    float observedArea = GetObservedArea(fragNormal);

    vec4 specular = CalculateSpecular(fragNormal, normalize(worldPosition - cameraPosition));
    
    
    vec3 finalColor = (fragColor + specular.xyz) * observedArea;
    outColor = vec4(finalColor, 1);
}

float GetObservedArea(vec3 normal)
{
	// Set default light direction to downward
	vec3 lightDirection = vec3(0, -1, 0);

	// If the light is a directional light, use its direction
	if(light.type == 0)
	{
		lightDirection = normalize(light.direction);
	}

	// Calculate dot product between normal and negative light direction
	float dotProduct = dot(normal, -lightDirection);

	// Clamp dot product and return it
    float observedArea = clamp(dotProduct, minOA, maxOA);
    return observedArea;
}

vec4 CalculateSpecular(vec3 normal, vec3 viewDirection)
{
	vec3 reflected = reflect(light.direction, normal);

	float cosAngle = clamp(dot(reflected, viewDirection), 0, 1);

	float exp = texture(glossMap, fragTexCoord).r;

	float phongSpecular = pow(cosAngle, exp);

	return texture(specMap, fragTexCoord) * phongSpecular;
}