#version 450

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec4 fragpos;
layout(location = 6) in vec4 viewPos;
layout(location = 7) in vec3 viewNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outViewNormal;
layout(location = 2) out vec4 outViewPos;

void main()
{
	vec3 finalColor = fragColor;

	outColor = vec4(finalColor, 1);

    float alphaThreshold = 0.1;
    if(outColor.w < alphaThreshold)
    {
        discard;
    } 

    outViewNormal = vec4(normalize(viewNormal), 1);
    outViewPos = viewPos;
}
