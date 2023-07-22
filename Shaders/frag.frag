#version 450

layout(location = 0) in vec2 Current_Position;
layout(location = 0) out vec4 Output;
layout(binding = 1) uniform sampler2D Texture;

// get (x, y) from: vec4 gl_FragCoord

void main() {
    Output = texture(Texture, Current_Position);
}   