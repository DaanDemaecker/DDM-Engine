#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;

layout(set = 0, binding = 2) uniform sampler2D inPos;



layout(location = 0) out float outValue;



layout(location = 0) in vec2 fragUV;

void CalculateOutValue();
float ComputeAO(vec3 fragPos, vec3 normal, vec2 direction, vec2 stepSize);

const int directions = 8; // Number of directions to test
const int samples = 8; // Number of samples per direction
const float pi = 3.14159265358979323846; // Pi constant
const float bias = radians(15.0); // Bias angle in radians
const float inf = 1/0; // Constant representing infinity
const float radius = 1; // Constant radius of testing dome

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
    // Read normal and fragment position from respective textures
   vec3 normal = normalize(subpassLoad(inNormal).xyz);
   vec3 fragPos = texture(inPos, fragUV).xyz;

   // Get the screen size and use it to calculate stepsize in UV space per sample
   vec2 screenSize = vec2(textureSize(inPos, 0).xy);
   vec2 stepSize = vec2(1.0) / screenSize;
   vec2 scaledStep = stepSize * radius;

   float angle = 2*pi/directions;

   float occlusion = 0;

   for(int i = 0; i < directions; ++i)
   {
        vec2 direction = normalize(vec2(cos(angle * i), sin(angle * i)));       

        occlusion += ComputeAO(fragPos, normal, direction, scaledStep);
   }

   occlusion /= directions;

   occlusion = clamp(occlusion, 0, 1);

   outValue = 1 - occlusion;
}


float ComputeAO(vec3 fragPos, vec3 normal, vec2 direction, vec2 stepSize)
{
    // Calculate tangent angle from normal (upward-facing hemisphere)
    vec3 leftDirection = cross(normal, vec3(direction, 0.0));
    vec3 tangent = normalize(cross(leftDirection, normal));

    // Tangent angle to bias the dome upward
    float tangentAngle = atan(-tangent.z, length(tangent.xy));
    float sinTangentAngle = sin(tangentAngle);

    float highestValue = -inf;
    
    float horizonAngle = 0.0f;

    for (int i = 1; i <= samples; ++i)
    {
        vec2 offsetUV = clamp(fragUV + direction * (i * stepSize), vec2(0.0), vec2(1.0));
        vec3 samplePos = texture(inPos, offsetUV).xyz;

        if(samplePos.z > highestValue)
        {
        
            vec3 difference = samplePos - fragPos;
            float xyLength = length(difference.xy);

            float epsilon = 0.01;
            if(xyLength <= epsilon)
            {
                continue;
            }

            
            highestValue = samplePos.z;

            horizonAngle = atan(-difference.z, length(difference.xy));
        }
    }

    if(highestValue == -inf)
    {
        horizonAngle = tangentAngle;
    }


    float sinHorizonAngle = sin(horizonAngle);
    return sinHorizonAngle - sinTangentAngle;
}
