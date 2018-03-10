#version 410 core

layout (location = 0) in vec3 position;

// TODO accept modelToWorld transform as uniform here and multiply input pos by it

out vec3 TCworldPos;

void main() {
    TCworldPos = position;
}
