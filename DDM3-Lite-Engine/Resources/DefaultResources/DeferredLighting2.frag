#version 450

layout(binding = 0) uniform sampler2D gAlbedo;

layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 fragUV;

void main()
{
    outColor = texture(gAlbedo, fragUV);
}