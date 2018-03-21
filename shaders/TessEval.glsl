#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];
in float TETessLevel[];

out vec3 fragPos;

uniform mat4 viewProj;

// TODO don't copy-paste this from vertex.glsl but use some include or something
float getHeightAt(vec2 xzPos) {
    float heightHere = noise1(xzPos);
    float h2 = noise1(xzPos * 2);
    float h3 = noise1(xzPos * 4);
    float h4 = noise1(xzPos * 8);
    // TODO make these factors uniforms so that they can be edited at runtime
    return heightHere * 1.0 + h2 * 0.15 + h3 * 0.01 + h4 * 0.005;
}

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];

    posFromBary.y = getHeightAt(posFromBary.xz);
    gl_Position = viewProj * vec4(posFromBary, 1.0);
    fragPos = posFromBary;
}
