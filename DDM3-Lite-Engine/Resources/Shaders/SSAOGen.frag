#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inPosition;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput depth;

layout(set = 0, binding = 3) uniform Samples {
    vec3 samples[64];
} sampleList;

layout(location = 0) out float outValue;


layout(location = 0) in vec2 fragUV;

void main()
{
    if(subpassLoad(depth).r == 1)
    {
       outValue = sampleList.samples[0].y;
    }
    else
    {
        outValue = 0.1;
    }
}