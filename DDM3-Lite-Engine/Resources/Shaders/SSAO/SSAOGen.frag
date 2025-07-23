#version 450

const int sampleAmount = 64;
const float radius = 0.5;
const float bias = 0.0025;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inNormal;
layout (input_attachment_index = 2, set = 0, binding = 1) uniform subpassInput depth;


layout(set = 0, binding = 2) uniform sampler2D inPos;
layout(set = 0, binding = 3) uniform sampler2D noiseTexture;

layout(set = 0, binding = 4) uniform Samples {
    vec4 samples[sampleAmount];
} sampleList;

layout(set = 0, binding = 5) uniform Projection {
    mat4 projection;
} projectionMatrix;


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
        CalculateOutValue();
    }
}

// Logic by Brian will
// https://www.youtube.com/watch?v=7hxrPKoELpo
void CalculateOutValue()
{
    // Get input for SSAO algorithm

    vec3 pos = texture(inPos, fragUV).xyz;
    vec3 normal = normalize(subpassLoad(inNormal).xyz);
    vec3 randomVec = normalize(texture(noiseTexture, fragUV).xyz);

    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));

    vec3 biTangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, biTangent, normal);

    // iterate over the samples and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < sampleAmount; ++i)
    {
      vec3 sampleVec = TBN * sampleList.samples[i].xyz;

      sampleVec = pos + sampleVec * radius;

      vec4 offset = vec4(sampleVec, 1.0);
      offset = projectionMatrix.projection * offset;
      offset.xyz /= offset.w;
      offset.xyz = offset.xyz * 0.5 + 0.5;

      vec3 occluderPos = texture(inPos, offset.xy).rgb;

      float rangeCheck = smoothstep(0.0, 1.0, radius/length(pos - occluderPos));

      occlusion += (occluderPos.z <= sampleVec.z - bias ? 1.0 : 0.0) * rangeCheck;
    }

   outValue = 1.0 - (occlusion / sampleAmount);
}