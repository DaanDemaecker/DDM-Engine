#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 boneIndices;
layout(location = 6) in vec4 boneWeights;
layout(location = 7) in float inUvSetIndex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float uvSetIndex;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragTangent;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
	uvSetIndex = inUvSetIndex;

	mat3 transposeMat = mat3(transpose(inverse(ubo.model)));
	
	fragNormal = normalize(transposeMat * normal);
    fragTangent = normalize(transposeMat * tangent);
}