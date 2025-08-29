#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformLightObject {
    int type;
	float intensity;
	float range;
	float angle;
    vec3 direction;
    vec3 position;
	vec3 color;
} light;

float GetObservedArea(vec3 normal);

float minOA = 0.02;
float maxOA = 1;

void main()
{
    float observedArea = GetObservedArea(fragNormal);

	outColor = vec4(fragColor * observedArea * light.color * light.intensity, 1);
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