#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];

uniform mat4 viewProj;

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];
    float heightHere = noise1(posFromBary.xz);
    // TODO make 1.0 here a uniform
    posFromBary.y = heightHere * 1.0;
    gl_Position = viewProj * vec4(posFromBary, 1.0);
}
