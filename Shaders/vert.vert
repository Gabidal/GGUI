#version 450

vec2 UVs[6] = vec2[](
    vec2(0, 1),
    vec2(1, 1),
    vec2(0, 0),

    vec2(0, 0),
    vec2(1, 1),
    vec2(1, 0)
);

layout(location = 0) out vec2 UV;

vec2 positions[6] = vec2[](
    vec2(-1, -1),
    vec2( 1, -1),
    vec2(-1,  1),

    vec2(-1,  1),
    vec2( 1, -1),
    vec2( 1,  1)
    
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    UV = UVs[gl_VertexIndex];
}