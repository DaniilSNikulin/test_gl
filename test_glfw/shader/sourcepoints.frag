#version 430

layout(binding=0) uniform sampler2D PositionTex;

in vec3 WorldPosition;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 WorldPositionData;

void main() {
    WorldPositionData = WorldPosition;
}
