#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 modelToWorld;

out vec3 TCworldPos;

// TODO don't copy-paste this from vertex.glsl but use some include or something
float getHeightAt(vec2 xzPos) {
    xzPos *= 0.05;
    float heightHere = noise1(xzPos * 8);
    float h0 = noise1(xzPos);
    float h2 = noise1(xzPos * 64);
    float h3 = noise1(xzPos * 4);
    // TODO make these factors uniforms so that they can be edited at runtime
    return h0 * 16.0 + heightHere * 1.0 + h2 * 0.05 + h3 * 0.25;
}

void main() {
    vec4 worldPos = modelToWorld * vec4(position, 1.0);
    float heightHere = getHeightAt(worldPos.xz);
    worldPos.y = heightHere;
    TCworldPos = vec3(worldPos);
}
