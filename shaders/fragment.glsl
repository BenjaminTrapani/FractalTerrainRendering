#version 410 core

in vec3 fragPos;

struct TextureGroup {
    sampler2D diffuseMap;
    sampler2D normalMap;
};

// TODO blend between texture groups based on position or something cool
uniform TextureGroup textureGroups[1];

out vec4 color;

void main() {
    /*float colorScale = 150.0f / 255.0f;
    float colorOffset = 0.75 - 150.0f / 255.0f;
    vec2 rg1 = noise2(fragPos);
    vec2 rg2 = noise2(fragPos * 32);
    vec2 rg3 = noise2(fragPos * 64);
    vec2 rg4 = noise2(fragPos * 128);
    vec2 combinedRG = (rg1 * 0.4 + rg2 * 0.25 + rg3 * 0.20 + rg4 * 0.15) * colorScale + colorOffset;*/
    //vec2 combinedRG = vec2(1.0, 0.0);
    color = vec4(texture(textureGroups[0].diffuseMap, fragPos.xz).rgb, 1.0);
}
