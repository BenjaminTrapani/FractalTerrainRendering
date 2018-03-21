#version 410 core

layout (vertices = 3) out;

in vec3 TCworldPos[];
out vec3 TEworldPos[];
out float TETessLevel[];

uniform mat4 viewProj;
uniform vec2 screenSize;

vec4 projectVert(vec3 value) {
    vec4 projected = viewProj * vec4(value, 1.0);
    return projected / projected.w;
}
vec2 projectToScreen(vec4 projectedVert) {
    vec2 resultInXYPlane = vec2(projectedVert);
    resultInXYPlane += 1.0; //Now in range 0 to 2
    return resultInXYPlane * (screenSize * 0.5);
}

float computeTessFactor(vec2 screenV1, vec2 screenV2) {
    // hard-code to 16 pixels per tess factor for now
    return clamp(distance(screenV1, screenV2) / 16.0, 1.0, 128.0);
}

bool isVertexOffscreen(vec4 vert) {
    return any(lessThan(vert.xy, vec2(-2.0))) || any(greaterThan(vert.xy, vec2(2.0)));
}

void main() {
    TEworldPos[gl_InvocationID] = TCworldPos[gl_InvocationID];

    vec4 projectedVert0 = projectVert(TCworldPos[0]);
    vec4 projectedVert1 = projectVert(TCworldPos[1]);
    vec4 projectedVert2 = projectVert(TCworldPos[2]);

    bvec3 areVertsOffscreen = bvec3(isVertexOffscreen(projectedVert0),
                                    isVertexOffscreen(projectedVert1),
                                    isVertexOffscreen(projectedVert2));
    if (all(areVertsOffscreen)) {
        gl_TessLevelOuter[0] = 0;
        gl_TessLevelOuter[1] = 0;
        gl_TessLevelOuter[2] = 0;

        gl_TessLevelInner[0] = 0;
    } else {
        vec2 vert0Screen = projectToScreen(projectedVert0);
        vec2 vert1Screen = projectToScreen(projectedVert1);
        vec2 vert2Screen = projectToScreen(projectedVert2);

        float tessFac0 = computeTessFactor(vert1Screen, vert2Screen);
        float tessFac1 = computeTessFactor(vert2Screen, vert0Screen);
        float tessFac2 = computeTessFactor(vert0Screen, vert1Screen);

        gl_TessLevelOuter[0] = tessFac0;
        gl_TessLevelOuter[1] = tessFac1;
        gl_TessLevelOuter[2] = tessFac2;

        float averageTessFac = (tessFac0 + tessFac1 + tessFac2) / 3.0;
        TETessLevel[gl_InvocationID] = averageTessFac;
        gl_TessLevelInner[0] = averageTessFac;
    }
}
