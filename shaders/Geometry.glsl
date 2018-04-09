#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geomPos[3];

out vec3 tangentLightPos[1];
out vec3 tangentViewPos;
out vec3 fragPos;
out vec3 tangentFragPos;
out vec3 tangentSunDir;

uniform mat4 viewProj;
uniform vec3 cameraPos;
uniform vec3 lightPositions[1];
uniform vec3 sunDir;

void main() {
    vec3 edge1 = geomPos[1] - geomPos[0];
    vec3 edge2 = geomPos[2] - geomPos[0];

    vec2 uv1 = geomPos[0].xz;
    vec2 uv2 = geomPos[1].xz;
    vec2 uv3 = geomPos[2].xz;

    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 tangent;
    vec3 bitangent;

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent.z = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent = normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent.z = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent = normalize(bitangent);

    vec3 normalVec = cross(tangent, bitangent);

    mat3 tangentToWorld = mat3(tangent, bitangent, normalVec);
    mat3 worldToTangent = transpose(tangentToWorld);
    vec3 tempTangentViewPos = worldToTangent * cameraPos;
    vec3 tempTangentSunDir = worldToTangent * sunDir;

    for (int i = 0; i < gl_in.length(); i++) {
       gl_Position = viewProj * vec4(geomPos[i], 1.0);
       tangentLightPos[0] = worldToTangent * lightPositions[0];
       tangentViewPos = tempTangentViewPos;
       fragPos = geomPos[i];
       tangentFragPos = worldToTangent * geomPos[i];
       tangentSunDir = tempTangentSunDir;
       EmitVertex();
    }
}
