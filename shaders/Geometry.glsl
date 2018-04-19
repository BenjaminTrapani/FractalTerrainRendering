#version 410

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geomPos[6];

out vec3 tangentLightPos[1];
out vec3 tangentViewPos;
out vec3 fragPos;
out vec3 tangentFragPos;
out vec3 tangentSunDir;

uniform mat4 viewProj;
uniform vec3 cameraPos;
uniform vec3 lightPositions[1];
uniform vec3 sunDir;

mat3 getTBN(vec3 triangleEdges[3]) {
    vec3 edge1 = triangleEdges[1] - triangleEdges[0];
    vec3 edge2 = triangleEdges[2] - triangleEdges[0];

    vec2 uv1 = triangleEdges[0].xz;
    vec2 uv2 = triangleEdges[1].xz;
    vec2 uv3 = triangleEdges[2].xz;

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
    return tangentToWorld;
}

struct OutputGroup {
  vec3 tangentLightPos[1];
  vec3 tangentViewPos;
  vec3 tangentSunDir;
};

struct TriangleData {
  vec3 verts[3];
  mat3 worldToTangent;
  OutputGroup outputGroup;
};

void main() {
    // 1. Compute tbn matrix for this triangle and all adjacent triangles
    // 2. Compute tangent space coords for all outputs for all triangles
    // 2. For each vertex in this triangle:
    //        For each adjacent triangle sharing an edge with the vertex
    //            Average tangent space coords for each output and write output (emit vertex)
    // 
    TriangleData triangleData[4]; // First is the center triangle, the rest adjacent
    triangleData[0].verts[0] = geomPos[0]; 
    triangleData[0].verts[1] = geomPos[2];
    triangleData[0].verts[2] = geomPos[4];

    triangleData[1].verts[0] = geomPos[0]; 
    triangleData[1].verts[1] = geomPos[4];
    triangleData[1].verts[2] = geomPos[5];

    triangleData[2].verts[0] = geomPos[4]; 
    triangleData[2].verts[1] = geomPos[2];
    triangleData[2].verts[2] = geomPos[3];
    
    triangleData[3].verts[0] = geomPos[2]; 
    triangleData[3].verts[1] = geomPos[0];
    triangleData[3].verts[2] = geomPos[1];

    for (int i = 0; i < 4; ++i) {
      mat3 tangentToWorld = getTBN(triangleData[i].verts);
      triangleData[i].worldToTangent = transpose(tangentToWorld);

      triangleData[i].outputGroup.tangentLightPos[0] = triangleData[i].worldToTangent * lightPositions[0];
      triangleData[i].outputGroup.tangentViewPos = triangleData[i].worldToTangent * cameraPos;
      triangleData[i].outputGroup.tangentSunDir = triangleData[i].worldToTangent * sunDir;
    }

    float triangleDiffEpsilon = 0.0001f;
    for (int myVertIndx = 0; myVertIndx < 5; myVertIndx += 2) {
      vec3 totalFragPos = triangleData[0].worldToTangent * geomPos[myVertIndx];
      OutputGroup aggregateOutput = triangleData[0].outputGroup;
      for (int adjTri = 1; adjTri < 4; ++adjTri) {
        for (int adjTriIndex = 0; adjTriIndex < 3; ++adjTriIndex) {
          if (length(triangleData[adjTri].verts[adjTriIndex] - geomPos[myVertIndx]) < triangleDiffEpsilon) {
            aggregateOutput.tangentLightPos[0] += triangleData[adjTri].outputGroup.tangentLightPos[0];
            aggregateOutput.tangentViewPos += triangleData[adjTri].outputGroup.tangentViewPos;
            aggregateOutput.tangentSunDir += triangleData[adjTri].outputGroup.tangentSunDir;
            totalFragPos += triangleData[adjTri].worldToTangent * geomPos[myVertIndx];
            break;
          }
        }
      }
      aggregateOutput.tangentLightPos[0] /= 3;
      aggregateOutput.tangentViewPos /= 3;
      aggregateOutput.tangentSunDir /= 3;
      totalFragPos /= 3.0;

      gl_Position = viewProj * vec4(geomPos[myVertIndx], 1.0);
      tangentLightPos = aggregateOutput.tangentLightPos;
      tangentViewPos = aggregateOutput.tangentViewPos;
      fragPos = geomPos[myVertIndx];
      tangentFragPos = totalFragPos;
      tangentSunDir = aggregateOutput.tangentSunDir;
      EmitVertex();
    }

    /*mat3 tangentToWorld = mat3(tangent, bitangent, normalVec);
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
    }*/
}
