#version 450

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 0) uniform sampler2D gNormal;
layout(binding = 0) uniform sampler2D gPos;

layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 fragUV;

void main()
{
    outColor = texture(gNormal, fragUV);
}