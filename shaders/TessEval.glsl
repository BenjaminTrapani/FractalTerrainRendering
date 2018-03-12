#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 TEworldPos[];

uniform mat4 viewProj;

// returns a value between 0 and 1 that corresponds to the fractal value at worldPos
float evalFractalAtPos(vec3 worldPos) {
    // TODO make this a uniform
    const float pixelsPerFractalRepeat = 100.0f;

    const float mapFractalRepeatToXFactor = 3.5f / 10.0f;
    const float mapFractalRepeatToXOffset = -2.5f;
    const float mapFractalRepeatToYFactor = 2.0f / 10.0f;
    const float mapFractalRepeatToYOffset = -1.0f;

    float zx = mod(worldPos.x, pixelsPerFractalRepeat) * mapFractalRepeatToXFactor + mapFractalRepeatToXOffset;
    float zy = mod(worldPos.z, pixelsPerFractalRepeat) * mapFractalRepeatToYFactor + mapFractalRepeatToYOffset;

    const float cx = -0.8f;
    const float cy = 0.156f;

    float iteration = 0.0f;
    const float maxIterations = 1000.0f;
    while(zx*zx + zy*zy < 4.0f && iteration < maxIterations) {
        float xTemp = zx*zx - zy*zy;
        zy = 2*zx*zy + cy;
        zx = xTemp + cx;
        ++iteration;
    }
    if (iteration == maxIterations) {
        return 0.0f;
    }
    return iteration / maxIterations;
}

void main() {
    vec3 posFromBary = gl_TessCoord.x * TEworldPos[0] +
                       gl_TessCoord.y * TEworldPos[1] +
                       gl_TessCoord.z * TEworldPos[2];
    float heightHere = evalFractalAtPos(posFromBary);
    // TODO make 10 here a uniform
    posFromBary.y = heightHere * 1.0;
    gl_Position = viewProj * vec4(posFromBary, 1.0);
}
