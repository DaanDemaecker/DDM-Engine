#version 450

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gPos;
layout(binding = 2) uniform sampler2D gNormal;

layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 fragUV;

void main()
{
    outColor = texture(gAlbedo, fragUV);
}