#version 410 core

layout (vertices = 3) out;

in vec3 TCworldPos[];
out vec3 TEworldPos[];

uniform mat4 viewProj;
uniform vec2 screenSize;
uniform vec3 cameraPos;

vec4 projectVert(vec3 value) {
    vec4 projected = viewProj * vec4(value, 1.0);
    return projected / projected.w;
}

float computeTessFactor(float d1, float d2) {
    // hard-code to 32 pixels per tess factor for now
    return 128 / (d1 + d2);
}

bool isVertexOffscreen(vec4 vert) {
    return vert.z < -0.5 || any(lessThan(vert.xy, vec2(-1.5))) || any(greaterThan(vert.xy, vec2(1.5)));
}

void main() {
    TEworldPos[gl_InvocationID] = TCworldPos[gl_InvocationID];

    if (gl_InvocationID == 0) {
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
        float distToVert0 = distance(cameraPos, TCworldPos[0]);
        float distToVert1 = distance(cameraPos, TCworldPos[1]);
        float distToVert2 = distance(cameraPos, TCworldPos[2]);

        float tl0 = computeTessFactor(distToVert1, distToVert2);
        float tl1 = computeTessFactor(distToVert2, distToVert0);
        float tl2 = computeTessFactor(distToVert0, distToVert1);
        gl_TessLevelOuter[0] = tl0;
        gl_TessLevelOuter[1] = tl1;
        gl_TessLevelOuter[2] = tl2;
        gl_TessLevelInner[0] = (tl0 + tl1 + tl2) / 3.0;
      }
    }
}
