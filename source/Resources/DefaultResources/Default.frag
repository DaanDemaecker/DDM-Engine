#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

float GetObservedArea(vec3 normal);

void main()
{
	outColor = vec4(fragColor * GetObservedArea(fragNormal) * light.color * light.intensity, 1);
}

float GetObservedArea(vec3 normal)
{
	float dotProduct = dot(normal, -light.direction);
    float observedArea = clamp(dotProduct, 0, 1);
    return observedArea;
}