#version 410 core

in vec3 tangentLightPos[1];
in vec3 tangentViewPos;
in vec3 fragPos;
in vec3 tangentFragPos;
in vec3 tangentSunDir;

struct TextureGroup {
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D specularMap;

    float ambientFac;
    float shininess;
};

struct Light {
    // All light color components should be between 0 and 1.
    vec3 ambientColor;
    vec3 specularColor;
    vec3 diffuseColor;
};

// Order texture groups in increasing order by height. Blend between colors at indices. 0 - 2 are vertical
// colors with 2 as snow. 3 and 4 are alternates for 0 and 1 horizontally.
uniform TextureGroup textureGroups[5];
uniform int maxTextureGroupIndex = 4;
uniform int verticalTextureGroupCount = 3;
uniform int horizontalCycleOffset = 3; // Since 0 + 3 = 3, 1 + 3 = 4 as required

uniform Light lights[1]; // lights[0] is reserved for the sun, so its position is ignored

uniform float textureScaleLarge = 16.0f;
uniform float textureScaleSmall = 1.0f;
uniform float textureDistanceBlendFactor = 50.0f;
uniform float horizontalTextureCyclePeriod = 100.0f;
// blends between adjacent colors for (1 - horizontalBlendFactor) * (horizontalTextureCyclePeriod / 2) pixels
uniform float horizontalBlendFactor = 0.9f;
uniform float verticalBlendFactor = 0.9f;
uniform float terrainHeightDiff = 100.0f;
uniform float terrainHeightBias = 50.0f;

uniform float fogDensity = 0.01;
uniform vec3 fogColor = vec3(94.f/255.0f, 100.f/255.0f, 100.f/255.0f);
uniform float fogStartDistance = 20.0f;

out vec4 color;

vec3 getColorForGroupIndexAndCoords(int textureGroupIdx, vec2 sampleCoords, vec3 directionToViewer) {
    vec3 ambientContrib = lights[0].ambientColor * vec3(textureGroups[textureGroupIdx].ambientFac,
                                                    textureGroups[textureGroupIdx].ambientFac,
                                                    textureGroups[textureGroupIdx].ambientFac);

    vec3 unnormNormal = texture(textureGroups[textureGroupIdx].normalMap, sampleCoords).rgb;
    vec3 normal = normalize(unnormNormal * 2.0 - 1.0);

    vec3 diffuseColor = texture(textureGroups[textureGroupIdx].diffuseMap, sampleCoords).rgb;
    vec3 specularColor = texture(textureGroups[textureGroupIdx].specularMap, sampleCoords).rgb;

    vec3 diffuseContrib = diffuseColor * max(dot(tangentSunDir, normal), 0.0) * lights[0].diffuseColor;
    vec3 viewerLightMidpoint = normalize(directionToViewer + tangentSunDir);
    vec3 specularContrib = specularColor *
                                 pow(max(dot(viewerLightMidpoint, normal), 0.0), 
                                     textureGroups[textureGroupIdx].shininess) *
                                 lights[0].specularColor;
    vec3 prelimColor = min(diffuseContrib + specularContrib + ambientContrib, 1.0);
    return prelimColor;
}

vec3 getColorFromGroupIdx(int textureGroupIdx, float distToViewer, vec3 directionToViewer) {
    vec2 sampleCoordsHigh = fragPos.xz / textureScaleLarge;
    vec2 sampleCoordsLow = fragPos.xz / textureScaleSmall;
    vec3 longColor = getColorForGroupIndexAndCoords(textureGroupIdx, sampleCoordsHigh, directionToViewer);
    vec3 shortColor = getColorForGroupIndexAndCoords(textureGroupIdx, sampleCoordsLow, directionToViewer);
    vec3 mixedComp = mix(shortColor, longColor, clamp(distToViewer / textureDistanceBlendFactor, 0.0, 1.0));
    return mixedComp;
}

vec3 getBlendedColorForHorizontal(int flooredTextureGroupIdx, float param, float distToViewer, vec3 directionToViewer) {
    float halfCyclePeriod = horizontalTextureCyclePeriod / 2.0f;
    float blendSize = horizontalBlendFactor * halfCyclePeriod;
    float cyclePos = mod(param, horizontalTextureCyclePeriod);
    float halfCyclePos = mod(param, halfCyclePeriod);

    // Either 0 or 3 depending on cyclePos
    float srcOffset = floor((cyclePos / horizontalTextureCyclePeriod) + 0.5) * horizontalCycleOffset;
    float destOffset = mod(srcOffset + horizontalCycleOffset, 6);

    int finalSrcOffset = min(int(flooredTextureGroupIdx + srcOffset), maxTextureGroupIndex);
    int finalDestOffset = min(int(flooredTextureGroupIdx + destOffset), maxTextureGroupIndex);
    vec3 flooredColor = getColorFromGroupIdx(finalSrcOffset, distToViewer, directionToViewer);
    vec3 destColor = getColorFromGroupIdx(finalDestOffset, distToViewer, directionToViewer);

    float blendFact = (halfCyclePos - blendSize) / (halfCyclePeriod - blendSize);
    blendFact = clamp(blendFact, 0, 1);
    
    return mix(flooredColor, destColor, blendFact);
}

vec3 getBlendedColorAtPoint(float distToViewer, vec3 directionToViewer) {
    float rawTextureGroupIdx = clamp(((fragPos.y + terrainHeightBias) / terrainHeightDiff) * verticalTextureGroupCount, 
                                      0, (verticalTextureGroupCount - 1));
    int flooredTextureGroupIdx = int(rawTextureGroupIdx);
    int nextIndex = min(flooredTextureGroupIdx + 1, verticalTextureGroupCount - 1);
    
    vec3 flooredColor = mix(getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.x, distToViewer, directionToViewer),
                            getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.z, distToViewer, directionToViewer), 0.5);
    vec3 nextColor = mix(getBlendedColorForHorizontal(nextIndex, fragPos.x, distToViewer, directionToViewer),
                        getBlendedColorForHorizontal(nextIndex, fragPos.z, distToViewer, directionToViewer), 0.5);

    float blendFactor = (rawTextureGroupIdx - (flooredTextureGroupIdx + verticalBlendFactor)) / (1 - verticalBlendFactor);
    blendFactor = clamp(blendFactor, 0, 1);

    return mix(flooredColor, nextColor, blendFactor);
}

void main() {
    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    vec3 directionToViewer = normalize(directionToViewerUnnorm);
    float distToViewer = length(directionToViewerUnnorm);
    vec3 blendedColor = getBlendedColorAtPoint(distToViewer, directionToViewer);
    float fogFactor = 1.0 / exp((distToViewer - fogStartDistance) * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 finalColor = mix(fogColor, blendedColor, fogFactor);

    color = vec4(finalColor, 1.0);
}
