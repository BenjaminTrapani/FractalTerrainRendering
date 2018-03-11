//
// Created by Benjamin Trapani on 3/11/18.
//

#pragma once

#include "TerrainPatch.h"
#include <vector>
#include <glm/glm.hpp>

namespace FractalTerrain {
    class Terrain {
    public:
        using TerrainPatchGrid_t = std::vector<std::shared_ptr<TerrainPatch>>;

        Terrain(unsigned int patchWidth, unsigned int patchHeight, unsigned int patchTileSize);
        void updatePatchPositions(const glm::vec3& cameraPos);
        inline const TerrainPatchGrid_t& getTerrainPatches()const {
            return terrainPatches;
        }
    private:
        TerrainPatchGrid_t terrainPatches;
        const double halfPatchTileSize;
    };
}