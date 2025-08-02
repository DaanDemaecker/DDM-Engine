#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;

layout(set = 0, binding = 2) uniform sampler2D inPos;



layout(location = 0) out float outValue;

layout(location = 0) in vec2 fragUV;

void CalculateOutValue();
vec2 CalculateHorizonAngles(vec3 fragPos, vec3 viewDirection, vec2 testDirection, vec2 stepsize);


const int directions = 16; // Number of directions to test
const int samples = 16; // Number of samples per direction
const float pi = 3.14159265358979323846; // Pi constant
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
        outValue = 0.0f;
        CalculateOutValue();
    }
}

void CalculateOutValue()
{
    vec3 position = texture(inPos, fragUV).xyz;

    vec3 viewDirection = normalize(-position);
    vec3 normal = -normalize(subpassLoad(inNormal).xyz);

    float gamma = acos(clamp(dot(viewDirection, normal), -1.0, 1.0));


    // Get the screen size and use it to calculate stepsize in UV space per sample
   vec2 screenSize = vec2(textureSize(inPos, 0).xy);
   vec2 stepSize = vec2(1.0) / screenSize;
   vec2 scaledStep = stepSize * radius;

   float angle = pi/directions;

   float ao = 0;

   for(int i = 0; i < directions; ++i)
   {
        vec2 direction = normalize(vec2(cos(angle * i), sin(angle * i)));       

        vec2 horizonAngles = CalculateHorizonAngles(position, viewDirection, direction, scaledStep);

        float a = 0.25 * (-cos(2.0 * horizonAngles.x - gamma) + cos(gamma) + 2.0 * horizonAngles.x * sin(gamma))
                + 0.25 * (-cos(2.0 * horizonAngles.y - gamma) + cos(gamma) + 2.0 * horizonAngles.y * sin(gamma));

        ao += a;
   }

   ao = (1/pi) * (ao/float(directions));

   outValue = 1 - clamp(ao, 0.0, 1.0);

}

vec2 CalculateHorizonAngles(vec3 fragPos, vec3 viewDirection, vec2 testDirection, vec2 stepSize)
{
    vec2 horizonAngles = vec2(-inf);

    for (int i = 1; i <= samples; ++i)
    {
        vec2 offsetUV = testDirection * (i * stepSize);
        vec3 samplePos = texture(inPos, fragUV + offsetUV).xyz;

        vec3 directionToPixel = normalize(samplePos - fragPos);
        
        float horizonAngle = acos(dot(-viewDirection, directionToPixel));

        if(horizonAngle > horizonAngles.x)
        {
            horizonAngles.x = horizonAngle;
        }




        vec3 samplePos2 = texture(inPos, fragUV - offsetUV).xyz;

        vec3 directionToPixel2 = normalize(samplePos2 - fragPos);
        
        float horizonAngle2 = acos(dot(-viewDirection, directionToPixel2));

        if(horizonAngle2 > horizonAngles.y)
        {
            horizonAngles.y = horizonAngle2;
        }
    }

    return horizonAngles;
}