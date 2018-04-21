//
// Created by Benjamin Trapani on 3/11/18.
//
#include "Terrain.h"

using namespace FractalTerrain;

// How much of each unscaled patch should overlap? Prevents issues with seems
constexpr float overlapSize = 0.f;

Terrain::Terrain(unsigned int patchWidth, unsigned int patchHeight,
                 unsigned int patchTileSize, const float patchScale,
                 const unsigned int shaderID):
        terrainPatches(patchTileSize * patchTileSize),
        halfPatchTileSize(((double)patchTileSize - (overlapSize * patchTileSize)) * 0.5 * patchScale),
        quadSideLength(glm::vec2(1.f / (float)patchWidth, 1.f / (float)patchHeight) * patchScale * (1.f / 64.f)),
        terrainScale(patchScale) {
    for (unsigned int z = 0; z < patchTileSize; ++z) {
        for (unsigned int x = 0; x < patchTileSize; x++) {
            const glm::vec3 patchPos = glm::vec3(x - overlapSize, 0.0, z - overlapSize) * patchScale;
            auto newTerrainPatch = std::make_shared<TerrainPatch>(patchWidth, patchHeight, patchPos, patchScale);
            terrainPatches.at(z * patchTileSize + x) = newTerrainPatch;
        }
    }

    initTextures(shaderID);
}

void Terrain::initTextures(const unsigned int shaderID) {
    unsigned int slotIndex = 0;
    auto desertMountainGroup = std::make_shared<TextureGroup>("desert_mntn_d.ppm", "desert_mntn_n.ppm",
                                                              "desert_mntn_s.ppm", 0.1, 1.0,
                                                              slotIndex, shaderID);
    textures.push_back(desertMountainGroup);
    ++slotIndex;
}

void Terrain::bindTextures() {
    for (auto pTextureGroup : textures) {
        pTextureGroup->Bind();
    }
}

float Terrain::snapToNearest(const float input, const float interval) {
  const float numIntervalsInInput = input / interval;
  return floorf(numIntervalsInInput) * interval;
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
    /*for (auto terrainPatch : terrainPatches) {

      const glm::vec3 snappedTerrainPos(snapToNearest(cameraPos.x, quadSideLength.x),
                                 0.0f,
                                 snapToNearest(cameraPos.z, quadSideLength.y));
      terrainPatch->setPosition(snappedTerrainPos);
    }*/
}