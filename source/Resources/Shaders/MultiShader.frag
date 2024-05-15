#version 450

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(binding = 2) uniform UniformMultiShaderObject
{
    int diffuseAmount;
    uint diffuseEnabled;
} multiShaderObject;

layout(binding = 3) uniform sampler2D diffuseSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragTangent;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

void main()
{
    vec3 normal = fragNormal;
    
    float observedArea = GetObservedArea(fragNormal);

    vec3 finalColor = fragColor;

    if(multiShaderObject.diffuseEnabled != 0)
    {
	    finalColor = texture(diffuseSampler, fragTexCoord).rgb;
    }
	
	finalColor *= light.color * light.intensity * observedArea;

	outColor = vec4(finalColor, 1);

    float alphaThreshold = 0.1;
    if(outColor.w < alphaThreshold)
    {
        discard;
    } 
}

float GetObservedArea(vec3 normal)
{
    float dotProduct = dot(normal, -light.direction);
    float observedArea = clamp(dotProduct, 0, 1);
    return observedArea;
}