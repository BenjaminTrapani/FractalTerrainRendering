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

vec3 getBlendedColorForHorizontal(int flooredTextureGroupIdx, float param) {
    vec3 nextHorizColor;
    float cyclePeriod = 100.0f;
    float halfCyclePeriod = cyclePeriod / 2.0f;
    float blendSize = 0.75f * halfCyclePeriod; //blend for upper .25
    float cyclePos = mod(param, cyclePeriod);
    vec3 flooredColor;

    //0 and 1 should cycle to 3 and 4
    bool isOnAlternateNow = cyclePos >= halfCyclePeriod && flooredTextureGroupIdx < 2;
    if (isOnAlternateNow) {
      flooredColor = getColorFromGroupIdx(flooredTextureGroupIdx + 3);
    }else {
      flooredColor = getColorFromGroupIdx(flooredTextureGroupIdx);
    }
    
    float halfCyclePos = mod(param, halfCyclePeriod);
    if (flooredTextureGroupIdx < 2 && halfCyclePos >= blendSize) {
       vec3 destColor;
       if(isOnAlternateNow) {
          destColor = getColorFromGroupIdx(flooredTextureGroupIdx);
       } else {
          destColor = getColorFromGroupIdx(flooredTextureGroupIdx + 3);
       }
       float blendFact = (halfCyclePos - blendSize) / (halfCyclePeriod - blendSize);
       nextHorizColor = mix(flooredColor, destColor, blendFact);
   } else {
      nextHorizColor = flooredColor;
    }
    return nextHorizColor;
}

vec3 getBlendedColorAtPoint() {
    // 17.25 is the max offset of the terrain height, 2 is the number of texture groups.
    float terrainHeightDiff = 50.0;
    float halfTerrainHeightDiff = terrainHeightDiff / 2;
    int numTextureGroups = 3;

    float rawTextureGroupIdx = ((fragPos.y + halfTerrainHeightDiff) / terrainHeightDiff) * numTextureGroups;
    int flooredTextureGroupIdx = int(rawTextureGroupIdx);

    vec3 flooredColor = (getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.x)
                        + getBlendedColorForHorizontal(flooredTextureGroupIdx, fragPos.z)) / 2;
    
    vec3 blendedColor;
    // At blendSize, start blending the textures linearly. Can't be the largest texture group.
    float blendSize = 0.75;
    if (rawTextureGroupIdx - flooredTextureGroupIdx > blendSize && rawTextureGroupIdx + 1 < numTextureGroups) {
      vec3 nextColor = (getBlendedColorForHorizontal(flooredTextureGroupIdx + 1, fragPos.x)
                        + getBlendedColorForHorizontal(flooredTextureGroupIdx + 1, fragPos.z)) / 2;
      float blendFactor = (rawTextureGroupIdx - (flooredTextureGroupIdx + blendSize)) / (1 - blendSize);
      blendedColor = mix(flooredColor, nextColor, blendFactor);
    } else {
      blendedColor = flooredColor;
    }
    return blendedColor;
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
    /*vec3 fogColor = skyColor * 0.5;

    vec3 finalColor;
    if (distToViewer < 60.0) {
      float fogFactor = 1.0 / exp(distToViewer * fogDensity);
      fogFactor = clamp(fogFactor, 0.0, 1.0);
      finalColor = mix(fogColor, blendedColor, fogFactor);
    } else {
      //float scaledDistToViewer = (distToViewer - 50.0) / 20.0;
      //finalColor = mix(skyColor, fogColor, 1.0 - clamp(scaledDistToViewer, 0.0, 1.0));
      float fogFactor = 1.0 / exp((distToViewer - 60.0) * fogDensity * 10);
      fogFactor = clamp(fogFactor, 0.0, 1.0);
      finalColor = mix(skyColor, fogColor, fogFactor);
    }*/

    color = vec4(finalColor, 1.0);
    //color = vec4(0, 0, 0, 1.0);
}
