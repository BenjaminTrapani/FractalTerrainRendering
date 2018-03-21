#version 410 core

in vec3 fragPos;

out vec4 color;

void main() {
    float colorScale = 150.0f / 255.0f;
    vec2 rg1 = noise2(fragPos);
    vec2 rg2 = noise2(fragPos * 32);
    vec2 rg3 = noise2(fragPos * 64);
    vec2 rg4 = noise2(fragPos * 128);
    vec2 combinedRG = (rg1 * 0.4 + rg2 * 0.25 + rg3 * 0.20 + rg4 * 0.15) * colorScale;
    color = vec4(combinedRG, 0.0, 1.0f);
}
