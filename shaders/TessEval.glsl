#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];
in float TETessLevel[];

out vec3 geomPos;

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
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];

    posFromBary.y = getHeightAt(posFromBary.xz);
    geomPos = posFromBary;
}
