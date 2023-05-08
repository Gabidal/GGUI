#version 450

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D Input;



void main() {
    outColor = texture(Input, UV);
}