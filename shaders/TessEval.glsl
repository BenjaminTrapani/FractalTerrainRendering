#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];

out vec3 fragPos;

uniform mat4 viewProj;

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];
    float heightHere = noise1(posFromBary.xz);
    float h2 = noise1(posFromBary.xz * 8);
    float h3 = noise1(posFromBary.xz * 16);
    float h4 = noise1(posFromBary.xz * 32);
    // TODO make these factors uniforms so that they can be edited at runtime
    posFromBary.y = heightHere * 1.0 + h2 * 0.15 + h3 * 0.05 + h4 * 0.01;
    gl_Position = viewProj * vec4(posFromBary, 1.0);
    fragPos = posFromBary;
}
