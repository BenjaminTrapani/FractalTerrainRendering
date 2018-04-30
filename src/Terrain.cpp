//
// Created by Benjamin Trapani on 3/11/18.
//
#include "Terrain.h"

using namespace FractalTerrain;

// How much of each unscaled patch should overlap? Prevents issues with seams
constexpr float overlapSize = 0.f;

Terrain::Terrain(unsigned int ipatchWidth, unsigned int ipatchHeight,
                 unsigned int patchTileSize, const float patchScale,
                 const unsigned int shaderID):
        terrainPatches(patchTileSize * patchTileSize),
        halfPatchTileSize(((double)patchTileSize - (overlapSize * patchTileSize)) * 0.5 * patchScale),
        quadSideLength(glm::vec2(1.f / (float)ipatchWidth, 1.f / (float)ipatchHeight) * patchScale * (1.f / 64.f)),
        terrainScale(patchScale),
        // Two triangles per square and
        // (width - 1) * (height - 1) squares for a width x height set of vertices.
        // 3 vertices per triangle.
        indexBufferSize(3 * (ipatchHeight - 1) * (ipatchWidth - 1) * 2),
        patchWidth(ipatchWidth),
        patchHeight(ipatchHeight) {
    for (unsigned int z = 0; z < patchTileSize; ++z) {
        for (unsigned int x = 0; x < patchTileSize; x++) {
            const glm::vec3 patchPos = glm::vec3(x - overlapSize, 0.0, z - overlapSize) * patchScale;
            auto newTerrainPatch = std::make_shared<TerrainPatch>(patchWidth, patchHeight, patchPos, patchScale);
            terrainPatches.at(z * patchTileSize + x) = newTerrainPatch;
        }
    }

    initTextures(shaderID);
    genBuffers();
}

void Terrain::initTextures(const unsigned int shaderID) {
    unsigned int slotIndex = 0;

    auto grassyRocky = std::make_shared<TextureGroup>("grass_rocky_d.ppm", "grass_rocky_n.ppm",
                                                      "grass_rocky_s.ppm", 0.1, 1.0, slotIndex, shaderID);
    textures.push_back(grassyRocky);
    ++slotIndex;

    auto desertMountainGroup = std::make_shared<TextureGroup>("desert_mntn_d.ppm", "desert_mntn_n.ppm",
                                                              "desert_mntn_s.ppm", 0.1, 0.8,
                                                              slotIndex, shaderID);
    textures.push_back(desertMountainGroup);
    ++slotIndex;

    auto snowMountainGroup = std::make_shared<TextureGroup>("snow_mntn2_d.ppm", "snow_mntn2_n.ppm",
                                                              "snow_mntn2_s.ppm", 0.1, 4.0,
                                                              slotIndex, shaderID);
    textures.push_back(snowMountainGroup);
    ++slotIndex;

    auto groundFerns = std::make_shared<TextureGroup>("ground_ferns_d.ppm", "ground_ferns_n.ppm",
                                                              "ground_ferns_s.ppm", 0.1, 1.0,
                                                              slotIndex, shaderID);
    textures.push_back(groundFerns);
    ++slotIndex;

    auto jungleMntn = std::make_shared<TextureGroup>("jungle_mntn_d.ppm", "jungle_mntn_n.ppm",
                                                              "jungle_mntn_s.ppm", 0.1, 2.0,
                                                              slotIndex, shaderID);
    textures.push_back(jungleMntn);
    ++slotIndex;
}

void Terrain::bindTextures() {
    for (auto pTextureGroup : textures) {
        pTextureGroup->Bind();
    }
}

void Terrain::updatePatchPositions(const glm::vec3& cameraPos) {
    // Only consider camera x and z location.
    // For each patch:
    //  For each dimension (x and z):
    //    If (dist from camera to patch along dimension > terrainSizeOnDim / 2)
    //        patch position.dimension += terrainWidth (if on lower bound, - if on upper bound)
    for (auto terrainPatch: terrainPatches) {
        const glm::vec3& patchPos = terrainPatch->getPosition();
        // Get distance from camera to center of patch
        double xDistFromCam = abs(cameraPos.x - (patchPos.x + terrainScale / 2.0f));
        double zDistFromCam = abs(cameraPos.z - (patchPos.z + terrainScale / 2.0f));
        double xOffset = 0.0;
        double zOffset = 0.0;
        if (xDistFromCam > halfPatchTileSize) {
            xOffset = cameraPos.x > patchPos.x ? halfPatchTileSize * 2.0 : halfPatchTileSize * -2.0;
        }
        if (zDistFromCam > halfPatchTileSize) {
            zOffset = cameraPos.z > patchPos.z ? halfPatchTileSize * 2.0 : halfPatchTileSize * -2.0;
        }
        terrainPatch->setPosition(glm::vec3(patchPos.x + xOffset, patchPos.y, patchPos.z + zOffset));
    }
}

// Generate a patchWidth x patchHeight size set of vertices with triangulations in index buffer.
// Vertices have an origin at x = y = z = 0 and span along x and z from [0, 0] -> [1, 1]
std::shared_ptr<Terrain::VertexIndexBufferData> Terrain::generateVertexIndexBuffers() {
    const std::pair<float, float> spaceBetweenVerts(1.0f / (patchWidth - 1.0f), 1.0f / (patchHeight - 1.0f));
    // Layout (x y z), (x y z) ...
    auto vertexPositions = std::make_shared<std::vector<float>>(patchWidth * patchHeight * 3);
    unsigned int vertedBufferIndex = 0;
    for (unsigned int z = 0; z < patchHeight; ++z) {
        for (unsigned int x = 0; x < patchWidth; ++x) {
            const float xPos = (float)x * spaceBetweenVerts.first;
            const float zPos = (float)z * spaceBetweenVerts.second;
            vertexPositions->at(vertedBufferIndex++) = xPos;
            vertexPositions->at(vertedBufferIndex++) = 0.0f;
            vertexPositions->at(vertedBufferIndex++) = zPos;
        }
    }

    // Triangulation. Operate on sets of 4 vertices in a square. Triangulate top left and then bottom right.
    auto indices = std::make_shared<std::vector<GLuint>>(indexBufferSize);
    unsigned int indexBufferIndex = 0;
    for (unsigned int z = 0; z < patchHeight - 1; ++z) {
        for (unsigned int x = 0; x < patchWidth - 1; ++x) {
            // Triangle anchored on lower left
            indices->at(indexBufferIndex++) = z * patchWidth + x;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x + 1;

            // Triangle anchored on upper right
            indices->at(indexBufferIndex++) = z * patchWidth + x + 1;
            indices->at(indexBufferIndex++) = z * patchWidth + x;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x + 1;
        }
    }
    return std::make_shared<VertexIndexBufferData>(vertexPositions, indices);
};

void Terrain::genBuffers() {
    auto const terrainData = generateVertexIndexBuffers();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, terrainData->vertexData->size() * sizeof(terrainData->vertexData->at(0)),
                 terrainData->vertexData->data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainData->indexData->size() * sizeof(terrainData->indexData->at(0)),
                 terrainData->indexData->data(), GL_STATIC_DRAW);
}

void Terrain::bindVertexBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}
void Terrain::bindIndexBuffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

GLsizei Terrain::getIndexBufferCount() const {
    return indexBufferSize;
}