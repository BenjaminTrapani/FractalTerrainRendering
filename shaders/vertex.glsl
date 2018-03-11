#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 modelToWorld;

out vec3 TCworldPos;

void main() {
    TCworldPos = vec3(modelToWorld * vec4(position, 1.0));
}
