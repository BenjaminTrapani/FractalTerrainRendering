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
uniform TextureGroup textureGroups[1];
uniform Light lights[1]; // lights[0] is reserved for the sun, so its position is ignored
uniform vec3 ambientLightColor;

out vec4 color;

void main() {
    vec2 sampleCoords = fragPos.xz;

    vec3 ambientContrib = ambientLightColor * vec3(textureGroups[0].ambientFac,
                                                    textureGroups[0].ambientFac,
                                                    textureGroups[0].ambientFac);

    vec3 unnormNormal = texture(textureGroups[0].normalMap, sampleCoords).rgb;
    vec3 normal = normalize(unnormNormal * 2.0 - 1.0);
    //vec3 normal = normalize(unnormNormal);

    vec3 diffuseColor = texture(textureGroups[0].diffuseMap, sampleCoords).rgb;
    vec3 specularColor = texture(textureGroups[0].specularMap, sampleCoords).rgb;

    vec3 diffuseContrib = diffuseColor * max(dot(tangentSunDir, normal), 0.0) * lights[0].diffuseColor;
    vec3 directionToViewerUnnorm = tangentViewPos - tangentFragPos;
    vec3 directionToViewer = normalize(directionToViewerUnnorm);
    vec3 viewerLightMidpoint = normalize(directionToViewer + tangentSunDir);
    vec3 specularContrib = specularColor *
                                 pow(max(dot(viewerLightMidpoint, normal), 0.0), 
                                     textureGroups[0].shininess) *
                                 lights[0].specularColor;
    vec3 prelimColor = min(diffuseContrib + specularContrib + ambientContrib, 1.0);

    float distToViewer = length(directionToViewerUnnorm);
    float fogDensity = 0.05f;
    float fogFactor = 1.0 / exp(distToViewer * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogColor = vec3(135.f/255.0f, 206.f/255.0f, 250.f/255.0f);
    // mix function fogColor⋅(1−fogFactor) + lightColor⋅fogFactor
    vec3 finalColor = mix(fogColor, prelimColor, fogFactor);

    color = vec4(finalColor, 1.0);
}
