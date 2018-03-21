#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];
in float TETessLevel[];

out vec3 fragPos;

uniform mat4 viewProj;

// TODO don't copy-paste this from vertex.glsl but use some include or something
float getHeightAt(vec2 xzPos) {
    float heightHere = noise1(xzPos);
    float h2 = noise1(xzPos * 8);
    float h3 = noise1(xzPos * 16);
    float h4 = noise1(xzPos * 32);
    // TODO make these factors uniforms so that they can be edited at runtime
    return heightHere * 1.0 + h2 * 0.15 + h3 * 0.05 + h4 * 0.01;
}

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];

    float weightedTessLevel = gl_TessCoord.x * TETessLevel[0] +
                              gl_TessCoord.y * TETessLevel[1] +
                              gl_TessCoord.z * TETessLevel[2];
    // lerp along TETessLevel - floor(TETessLevel) to determine height
    float progressIntoTessLevel = weightedTessLevel - floor(weightedTessLevel);
    float destHeight = getHeightAt(posFromBary.xz);
    posFromBary.y = mix(posFromBary.y, destHeight, progressIntoTessLevel);
    //posFromBary.y = getHeightAt(posFromBary.xz);
    gl_Position = viewProj * vec4(posFromBary, 1.0);
    fragPos = posFromBary;
}
