#version 410 core

layout (vertices = 3) out;

in vec3 TCworldPos[];
out vec3 TEworldPos[];

uniform mat4 viewProj;
uniform vec2 screenSize;

vec2 projectToScreen(vec3 value) {
    vec4 result = viewProj * vec4(value, 1.0);
    result /= result.w;
    vec2 resultInXYPlane = vec2(result);
    resultInXYPlane += 1.0; //Now in range 0 to 2
    return resultInXYPlane * (screenSize * 0.5);
}

float computeTessFactor(vec2 screenV1, vec2 screenV2) {
    return clamp(distance(screenV1, screenV2) / 32.0, 1.0, 512.0);
}

void main() {
    TEworldPos[gl_InvocationID] = TCworldPos[gl_InvocationID];

    vec2 vert0Screen = projectToScreen(TCworldPos[0]);
    vec2 vert1Screen = projectToScreen(TCworldPos[1]);
    vec2 vert2Screen = projectToScreen(TCworldPos[2]);

    gl_TessLevelOuter[0] = computeTessFactor(vert1Screen, vert2Screen);
    gl_TessLevelOuter[1] = computeTessFactor(vert2Screen, vert0Screen);
    gl_TessLevelOuter[2] = computeTessFactor(vert0Screen, vert1Screen);

    gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) / 3.0;
}
