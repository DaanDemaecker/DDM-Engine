#version 450

vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragUV = 0.5 * (gl_Position.xy + 1.0); // Convert to UV [0,1]
}