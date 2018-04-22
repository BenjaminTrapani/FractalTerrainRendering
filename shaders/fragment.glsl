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
uniform TextureGroup textureGroups[3];
uniform Light lights[1]; // lights[0] is reserved for the sun, so its position is ignored
uniform vec3 ambientLightColor;

out vec4 color;

vec3 getColorFromGroupIdx(int textureGroupIdx) {
    vec2 sampleCoords = fragPos.xz;

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

void main() {
    // 17.25 is the max offset of the terrain height, 2 is the number of texture groups.
    float terrainHeightDiff = 17.25;
    float halfTerrainHeightDiff = terrainHeightDiff / 2;
    int numTextureGroups = 3;

    float rawTextureGroupIdx = ((fragPos.y + halfTerrainHeightDiff) / terrainHeightDiff) * numTextureGroups;
    int flooredTextureGroupIdx = int(rawTextureGroupIdx);
    vec3 flooredColor = getColorFromGroupIdx(flooredTextureGroupIdx);
    
    vec3 blendedColor = flooredColor;
    // At 0.9, start blending the textures linearly. Can't be the largest texture group.
    float blendSize = 0.65;
    if (rawTextureGroupIdx - flooredTextureGroupIdx > blendSize && rawTextureGroupIdx + 1 < numTextureGroups) {
      vec3 nextColor = getColorFromGroupIdx(flooredTextureGroupIdx + 1);
      float blendFactor = (rawTextureGroupIdx - (flooredTextureGroupIdx + blendSize)) / (1 - blendSize);
      blendedColor = mix(flooredColor, nextColor, blendFactor);
    }

    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    float distToViewer = length(directionToViewerUnnorm);
    float fogDensity = 0.05f;
    float fogFactor = 1.0 / exp(distToViewer * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogColor = vec3(135.f/255.0f, 206.f/255.0f, 250.f/255.0f) * 0.5;
    vec3 finalColor = mix(fogColor, blendedColor, fogFactor);

    color = vec4(finalColor, 1.0);
}
