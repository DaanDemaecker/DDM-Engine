#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;

layout(set = 0, binding = 2) uniform sampler2D inPos;



layout(location = 0) out float outValue;



layout(location = 0) in vec2 fragUV;

void CalculateOutValue();


void main()
{
    if(subpassLoad(depth).r == 1)
    {
       outValue = 1.0;
    }
    else
    {
        outValue = 0.0f;
        CalculateOutValue();
    }
}

void CalculateOutValue()
{
   
}