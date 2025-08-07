#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inputPosition;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput aoMap;

//layout(set = 0, binding = 4) uniform View {
//    mat4 view;
//} viewMatrix;

layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 fragUV;

void main()
{
   vec3 color = subpassLoad(inputColor).rgb;

    float aoMapValue = subpassLoad(aoMap).r;

    color *= aoMapValue;

//    vec3 viewNormal = subpassLoad(inputNormal).xyz;
//    mat3 viewToWorld = transpose(mat3(viewMatrix.view)); // assuming no non-uniform scale
//    vec3 worldNormal = normalize(viewToWorld * viewNormal);

    // Flip x and z normal
    //worldNormal.x = -worldNormal.x;
    //worldNormal.z = -worldNormal.z;

    outColor = vec4(color, 1.0);
}