#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inputPosition;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput aoMap;

layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 fragUV;

void main()
{
    vec3 color = subpassLoad(inputColor).rgb;

    float aoMapValue = subpassLoad(aoMap).r;

    color *= aoMapValue;

    outColor = vec4(color, 1);
}