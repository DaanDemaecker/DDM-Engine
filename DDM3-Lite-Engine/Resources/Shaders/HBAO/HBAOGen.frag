#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;

layout(set = 0, binding = 2) uniform sampler2D inPos;



layout(location = 0) out float outValue;



layout(location = 0) in vec2 fragUV;

void CalculateOutValue();
float ComputeAO(vec3 fragPos, vec3 normal, vec2 direction, vec2 screenSize);

const int directions = 8; // Number of directions to test
const int samples = 8; // Number of samples per direction
const float pi = 3.14159265358979323846; // Pi constant
const float bias = radians(30.0); // Bias angle in radians
const float inf = 1/0; // Constant representing infinity

void main()
{
    if(subpassLoad(depth).r == 1)
    {
       outValue = 1.0;
    }
    else
    {
        CalculateOutValue();
    }
}

void CalculateOutValue()
{
   vec2 screenSize = vec2(textureSize(inPos, 0).xy);
   vec3 normal = normalize(subpassLoad(inNormal).xyz);
   vec3 fragPos = texture(inPos, fragUV).xyz;

   float angle = pi/directions;

   float ao = 0;

   for(int i = 0; i < directions; ++i)
   {
        vec2 direction = vec2(cos(angle * i), sin(angle * i));

        ao += ComputeAO(texture(inPos, fragUV).xyz, normal, direction, screenSize);
        
   }

   ao /= directions;
   ao = clamp(ao, 0, 1); // Clamp the AO value to [0, 1]
   outValue = 1 - ao;
}


float ComputeAO(vec3 fragPos, vec3 normal, vec2 direction, vec2 screenSize)
{
    vec2 stepSize = vec2(1.0) / screenSize;


    vec3 leftDirection = cross(normal, vec3(direction, 0.0));
    vec3 tangent = normalize(cross(leftDirection, normal));

    float tangentAngle = atan(tangent.z / length(tangent.xy));
    float sinTangentAngle = sin(tangentAngle + bias);


    
    vec3 highestVector = vec3(-inf);
    float highestAngle = -inf;

    for (int i = 1; i <= samples; ++i)
    {
        vec2 offsetUV = fragUV + direction * (i * stepSize) / screenSize;
        vec3 samplePos = texture(inPos, offsetUV).xyz;

        vec3 fragPosNormal = normalize(samplePos);
        float dist = length(tangent-fragPosNormal);


        if(dist > highestAngle)
        {
            highestAngle = dist;
            highestVector = samplePos;
        }
    }

    vec3 horizonVector = (highestVector - fragPos);
    float horizonAngle = atan(horizonVector.z / length(horizonVector.xy));
    float sinHorizonAngle = sin(horizonAngle);

    return sinHorizonAngle - sinTangentAngle;
}