#version 410 core

layout (vertices = 3) out;

in vec3 TCworldPos[];
out vec3 TEworldPos[];

uniform vec3 cameraWorldPos;

float computeTessFactor(float dist1, float dist2) {
    float averageDistance = (dist1 + dist2) * 0.5;
    // TODO replace 5.0 and 10.0 with uniforms
    float finalFactor = min(5.0 / averageDistance, 10.0);
    return finalFactor;
}

void main() {
    TEworldPos[gl_InvocationID] = TCworldPos[gl_InvocationID];

    float vert0Distance = distance(cameraWorldPos, TEworldPos[0]);
    float vert1Distance = distance(cameraWorldPos, TEworldPos[0]);
    float vert2Distance = distance(cameraWorldPos, TEworldPos[0]);

    gl_TessLevelOuter[0] = computeTessFactor(vert1Distance, vert2Distance);
    gl_TessLevelOuter[1] = computeTessFactor(vert2Distance, vert0Distance);
    gl_TessLevelOuter[2] = computeTessFactor(vert0Distance, vert1Distance);

    gl_TessLevelInner[0] = max(max(gl_TessLevelOuter[0], gl_TessLevelOuter[1]),  gl_TessLevelOuter[2]);
}
