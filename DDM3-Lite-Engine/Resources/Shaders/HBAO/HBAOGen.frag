#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;

layout(set = 0, binding = 2) uniform sampler2D inPos;



layout(location = 0) out float outValue;



layout(location = 0) in vec2 fragUV;


void CalculateOutValue();
float ComputeAO(vec3 normal, vec2 direction, vec2 screenSize, vec3 fragPos);
vec3 GetHighestPos(float texelSize, vec2 direction, vec3 tangent, vec3 fragPos);

const int directions = 8; // Number of directions to test
const int samples = 8; // Number of samples per direction
const float pi = 3.14159265358979323846; // Pi constant
const float bias = radians(30.0); // Bias angle in radians
float posInf = 1.0 / 0.0; // Positive infinity constant
float negInf = -1.0 / 0.0; // Negative infinity constant

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

   float rez = 0;

   for(int i = 0; i < directions; ++i)
   {
        vec2 direction = vec2(cos(angle * i), sin(angle * i));

        rez += ComputeAO(normal, direction, screenSize, texture(inPos, fragUV).xyz);
        
   }

   rez /= directions;
   outValue = 1.0 - rez;

}


float ComputeAO(vec3 normal, vec2 direction, vec2 screenSize, vec3 fragPos)
{
    vec3 leftDirection = cross(normal, vec3(direction, 0));
    vec3 tangent = normalize(cross(leftDirection, normal));

    float tangentAngle = atan(tangent.z / length(tangent.xy));
    float sinTangentAngle = sin(tangentAngle + bias);
    
    float texelSize = 1.0 / screenSize.x;
    vec3 highestPos = GetHighestPos(texelSize, direction, tangent, fragPos);

    vec3 horizonVector = (highestPos - fragPos);
    float horizonAngle = atan(horizonVector.z / length(horizonVector.xy));
    float sinHorizonAngle = sin(horizonAngle);

    return clamp(1, 0, sinHorizonAngle - sinTangentAngle);
}

vec3 GetHighestPos(float texelSize, vec2 direction, vec3 tangent, vec3 fragPos)
{
    vec3 highestPos = vec3(0, 0, negInf);
    float highestDepth = negInf;

    for(int i = 1; i <= samples; ++i)
    {
        vec2 marchPosition = fragUV + i * texelSize * direction;

        vec3 fragPosMarch = texture(inPos, marchPosition).xyz;

        // find the distance to the tangent
        vec3 fragPosNormal = normalize(fragPosMarch);
        float dist = length(tangent - fragPosNormal);

        if(dist > highestDepth &&
        length(fragPosMarch - fragPos) < length(i*texelSize) * abs(fragPos.z)*2)
        {
            highestPos = fragPosMarch;
            highestDepth = dist;
        }
    }

    return highestPos;
}