#version 450

const int maxTextureAmount = 5;

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(binding = 2) uniform UniformMultiShaderObject
{
    int diffuseAmount;
    uint diffuseEnabled;
    int normalAmount;
    uint normalEnabled;
} multiShaderObject;

layout(binding = 3) uniform sampler2D diffuseSampler[maxTextureAmount];
layout(binding = 4) uniform sampler2D normalSampler[maxTextureAmount];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float uvSetIndex;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragTangent;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

vec3 CalculateNormal();

void main()
{
    vec3 normal = fragNormal;

     if(multiShaderObject.normalEnabled != 0 &&
    uvSetIndex <= multiShaderObject.normalAmount &&
    uvSetIndex < maxTextureAmount)
    {
        normal = CalculateNormal();
    }

    
    float observedArea = GetObservedArea(normal);

    vec3 finalColor = fragColor;

    if(multiShaderObject.diffuseEnabled != 0 &&
    uvSetIndex <= multiShaderObject.diffuseAmount &&
    uvSetIndex < maxTextureAmount)
        {
	        finalColor = texture(diffuseSampler[int(uvSetIndex)], fragTexCoord).rgb;
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

vec3 CalculateNormal()
{
    vec3 binormal = cross(fragTangent, fragNormal);
	
    mat3 tangentSpaceAxis = mat3(
        fragTangent,
        normalize(binormal),
        normalize(fragNormal)
    );
    
    vec3 normalColor = texture(normalSampler[int(uvSetIndex)], fragTexCoord).rgb;
    
    vec3 sampledNormal = 2.0 * normalColor - vec3(1.0);
    
    // Transform the normal to tangent space
    return normalize(tangentSpaceAxis * sampledNormal);
}