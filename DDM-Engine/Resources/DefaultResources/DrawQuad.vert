#version 450

const vec2 screenCorners[6] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, -1.0)
);

const vec2 uvs[6] = vec2[]
(
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 boneIndices;
layout(location = 6) in vec4 boneWeights;
layout(location = 7) in float inUvSetIndex;

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = vec4(screenCorners[gl_VertexIndex], 0.0, 1.0);
    fragUV = uvs[gl_VertexIndex];
}