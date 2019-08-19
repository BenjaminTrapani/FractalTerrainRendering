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

// BEGIN CODE I DID NOT WRITE. 
// Generate perlin noise dynamically.
// Unfortunately noise1 is slow and produces artifacts on some GPUs (intel iris pro for example)

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

// END CODE I DIDN'T WRITE

float getHeightAt(vec2 xzPos) {
    xzPos *= 0.02;
    float hn1 = snoise(xzPos * 0.1);
    float h0 = snoise(xzPos * 0.5);
    float h1 = snoise(xzPos * 2);
    float h2 = snoise(xzPos * 8);
    return (hn1 * 12.0 + h0 * 8.0 + h1 * 2 + h2 * 0.5) * 2.5;
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
