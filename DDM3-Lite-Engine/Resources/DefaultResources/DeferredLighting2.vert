#version 450

vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
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
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragUV = 0.5 * (gl_Position.xy + 1.0); // Convert to UV [0,1]
}