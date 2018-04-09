//
// Created by Benjamin Trapani on 3/11/18.
//
#include "Terrain.h"

using namespace FractalTerrain;

Terrain::Terrain(unsigned int patchWidth, unsigned int patchHeight,
                 unsigned int patchTileSize, const unsigned int shaderID):
        terrainPatches(patchTileSize * patchTileSize),
        halfPatchTileSize(((double)patchTileSize) * 0.5) {
    for (unsigned int z = 0; z < patchTileSize; ++z) {
        for (unsigned int x = 0; x < patchTileSize; x++) {
            auto newTerrainPatch = std::make_shared<TerrainPatch>(patchWidth, patchHeight);
            newTerrainPatch->setPosition(glm::vec3(x, 0.0, z));
            terrainPatches.at(z * patchTileSize + x) = newTerrainPatch;
        }
    }

    initTextures(shaderID);
}

void Terrain::initTextures(const unsigned int shaderID) {
    unsigned int slotIndex = 0;
    auto desertMountainGroup = std::make_shared<TextureGroup>("desert_mntn_d.ppm", "desert_mntn_n.ppm",
                                                              "desert_mntn_s.ppm", 0.1, 0.7,
                                                              slotIndex, shaderID);
    textures.push_back(desertMountainGroup);
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
        double xDistFromCam = abs(cameraPos.x - (patchPos.x + 0.5));
        double zDistFromCam = abs(cameraPos.z - (patchPos.z + 0.5));
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