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

// Order texture groups in increasing order by height. Blend between colors at indices
uniform TextureGroup textureGroups[5];
uniform Light lights[1]; // lights[0] is reserved for the sun, so its position is ignored
uniform vec3 ambientLightColor;

out vec4 color;

vec3 getColorForGroupIndexAndCoords(int textureGroupIdx, vec2 sampleCoords) {
    vec3 ambientContrib = ambientLightColor * vec3(textureGroups[textureGroupIdx].ambientFac,
                                                    textureGroups[textureGroupIdx].ambientFac,
                                                    textureGroups[textureGroupIdx].ambientFac);

    vec3 unnormNormal = texture(textureGroups[textureGroupIdx].normalMap, sampleCoords).rgb;
    vec3 normal = normalize(unnormNormal * 2.0 - 1.0);

    vec3 diffuseColor = texture(textureGroups[textureGroupIdx].diffuseMap, sampleCoords).rgb;
    vec3 specularColor = texture(textureGroups[textureGroupIdx].specularMap, sampleCoords).rgb;

    vec3 diffuseContrib = diffuseColor * max(dot(tangentSunDir, normal), 0.0) * lights[0].diffuseColor;
    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    vec3 directionToViewer = normalize(directionToViewerUnnorm);
    vec3 viewerLightMidpoint = normalize(directionToViewer + tangentSunDir);
    vec3 specularContrib = specularColor *
                                 pow(max(dot(viewerLightMidpoint, normal), 0.0), 
                                     textureGroups[textureGroupIdx].shininess) *
                                 lights[0].specularColor;
    vec3 prelimColor = min(diffuseContrib + specularContrib + ambientContrib, 1.0);
    return prelimColor;
}

vec3 getColorFromGroupIdx(int textureGroupIdx) {
    vec2 sampleCoordsHigh = fragPos.xz / 8.0;
    vec2 sampleCoordsLow = fragPos.xz;
    vec3 longColor = getColorForGroupIndexAndCoords(textureGroupIdx, sampleCoordsHigh);
    vec3 shortColor = getColorForGroupIndexAndCoords(textureGroupIdx, sampleCoordsLow);
    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    float distToViewer = length(directionToViewerUnnorm);
    vec3 mixedComp = mix(shortColor, longColor, clamp(distToViewer / 35, 0.0, 1.0));
    return mixedComp;
}

vec3 getBlendedColorForHorizontal(int flooredTextureGroupIdx, float param) {
    float cyclePeriod = 100.0f;
    float halfCyclePeriod = cyclePeriod / 2.0f;
    float blendSize = 0.9 * halfCyclePeriod;
    float cyclePos = mod(param, cyclePeriod);
    float halfCyclePos = mod(param, halfCyclePeriod);

    // Either 0 or 3 depending on cyclePos
    float srcOffset = floor((cyclePos / cyclePeriod) + 0.5) * 3;
    float destOffset = mod(srcOffset + 3, 6);
    int finalSrcOffset = min(int(flooredTextureGroupIdx + srcOffset), 4);
    int finalDestOffset = min(int(flooredTextureGroupIdx + destOffset), 4);
    vec3 flooredColor = getColorFromGroupIdx(finalSrcOffset);
    vec3 destColor = getColorFromGroupIdx(finalDestOffset);

    float blendFact = (halfCyclePos - blendSize) / (halfCyclePeriod - blendSize);
    blendFact = max(blendFact, 0);
    return mix(flooredColor, destColor, blendFact);
}

vec3 getBlendedColorAtPoint() {
    // 17.25 is the max offset of the terrain height, 2 is the number of texture groups.
    float terrainHeightDiff = 50.0;
    float halfTerrainHeightDiff = terrainHeightDiff / 2;
    int numTextureGroups = 3;

    float rawTextureGroupIdx = ((fragPos.y + halfTerrainHeightDiff) / terrainHeightDiff) * numTextureGroups;
    int flooredTextureGroupIdx = int(rawTextureGroupIdx);

    vec3 flooredColor = mix(getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.x),
                            getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.z), 0.5);
    
    vec3 blendedColor;
    // At blendSize, start blending the textures linearly. Can't be the largest texture group.
    float blendSize = 0.9;
    float blendFactor = (rawTextureGroupIdx - (flooredTextureGroupIdx + blendSize)) / (1 - blendSize);
    blendFactor = max(blendFactor, 0);
    vec3 nextColor = mix(getBlendedColorForHorizontal(min(flooredTextureGroupIdx + 1, 4), fragPos.x),
                      getBlendedColorForHorizontal(flooredTextureGroupIdx + 1, fragPos.z), 0.5);
    return mix(flooredColor, nextColor, blendFactor);
}

void main() {
    vec3 blendedColor = getBlendedColorAtPoint();

    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    float distToViewer = length(directionToViewerUnnorm);
    float fogDensity = 0.04f;
    vec3 skyColor = vec3(94.f/255.0f, 100.f/255.0f, 100.f/255.0f);
    float fogFactor = 1.0 / exp((distToViewer - 20) * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 finalColor = mix(skyColor, blendedColor, fogFactor);

    color = vec4(finalColor, 1.0);
    //color = vec4(0, 0, 0, 1.0);
}
