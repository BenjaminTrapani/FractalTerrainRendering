#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];
in float TETessLevel[];

uniform mat4 viewProj;
uniform vec3 cameraPos;
uniform vec3 lightPositions[1];
uniform vec3 sunDir;

out vec3 tangentLightPos[1];
out vec3 tangentViewPos;
out vec3 fragPos;
out vec3 tangentFragPos;
out vec3 tangentSunDir;

// TODO don't copy-paste this from vertex.glsl but use some include or something
float getHeightAt(vec2 xzPos) {
    xzPos *= 0.05;
    float heightHere = noise1(xzPos * 8);
    float h0 = noise1(xzPos);
    //float h2 = noise1(xzPos * 64);
    float h3 = noise1(xzPos * 4);
    // TODO make these factors uniforms so that they can be edited at runtime
    return h0 * 16.0 + heightHere * 1.0 /*+ h2 * 0.05*/ + h3 * 0.25;
}

mat3 getTBN(vec3 trianglePoints[3]) {
    vec3 edge1 = trianglePoints[1] - trianglePoints[0];
    vec3 edge2 = trianglePoints[2] - trianglePoints[0];

    vec2 uv1 = trianglePoints[0].xz;
    vec2 uv2 = trianglePoints[1].xz;
    vec2 uv3 = trianglePoints[2].xz;

    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 tangent;
    vec3 bitangent;

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = normalize(bitangent);

    vec3 normalVec = cross(tangent, bitangent);

    mat3 tangentToWorld = mat3(tangent, bitangent, normalVec);
    return tangentToWorld;
}

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];

    posFromBary.y = getHeightAt(posFromBary.xz);

    // estimate tbn here from 1 triangle surrounding posFromPary
    float triangleSideLen = 0.1f;
    float tan30 = 0.5773503f;
    float sin30 = 0.5;
    float x = tan30 * (triangleSideLen / 2);
    float h = x / sin30;

    vec3 topLeft = vec3(posFromBary.x - triangleSideLen / 2, 0, posFromBary.z - x);
    vec3 topRight = vec3(posFromBary.x + triangleSideLen / 2, 0, posFromBary.z - x);
    vec3 bottomCenter = vec3(posFromBary.x, 0, posFromBary.z + h);

    topLeft.y = getHeightAt(topLeft.xz);
    topRight.y = getHeightAt(topRight.xz);
    bottomCenter.y = getHeightAt(bottomCenter.xz);

    vec3 allVerts[3];
    allVerts[0] = bottomCenter;
    allVerts[1] = topRight;
    allVerts[2] = topLeft;
    mat3 tangentToWorld = getTBN(allVerts);
    mat3 worldToTangent = transpose(tangentToWorld);

    gl_Position = viewProj * vec4(posFromBary, 1.0);

    tangentLightPos[0] = worldToTangent * lightPositions[0];
    tangentViewPos = worldToTangent * cameraPos;
    fragPos = posFromBary;
    tangentFragPos = worldToTangent * posFromBary;
    tangentSunDir = worldToTangent * sunDir;
}
