//
// Created by Benjamin Trapani on 3/11/18.
//

#pragma once

#include "TerrainPatch.h"
#include <vector>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include "Texture.h"

namespace FractalTerrain {
    class Terrain {
    public:
        using TerrainPatchGrid_t = std::vector<std::shared_ptr<TerrainPatch>>;

        Terrain(unsigned int patchWidth,
                unsigned int patchHeight,
                unsigned int patchTileSize,
                unsigned int shaderID);
        void updatePatchPositions(const glm::vec3& cameraPos);
        void bindTextures();
        inline const TerrainPatchGrid_t& getTerrainPatches()const {
            return terrainPatches;
        }
    private:
        class TextureGroup {
        public:
            static constexpr unsigned int slotsPerGroup = 2;

            TextureGroup(const std::string& diffusePath,
                         const std::string& normalPath,
                         const unsigned int index,
                         const unsigned int shaderID): slotIndex(index * slotsPerGroup) {
                std::stringstream groupIndexedAccess;
                groupIndexedAccess << "textureGroups[" << index << "]";
                const std::string groupIndexedAccessStr = groupIndexedAccess.str();
                setupUniformNameForSlot(shaderID, groupIndexedAccessStr, "diffuseMap", slotIndex);
                setupUniformNameForSlot(shaderID, groupIndexedAccessStr, "normalMap", slotIndex + 1);
                diffuseMap.LoadTexture(diffusePath);
                normalMap.LoadTexture(normalPath);
            }
            void Bind() {
                diffuseMap.Bind(slotIndex);
                normalMap.Bind(slotIndex + 1);
            }
        private:
            Texture diffuseMap;
            Texture normalMap;
            const unsigned int slotIndex;

            void setupUniformNameForSlot(const unsigned int shaderID,
                                         const std::string& groupIndexedAccess,
                                         const std::string& propName,
                                         unsigned int index) {
                std::stringstream propCombined;
                propCombined << groupIndexedAccess << "." << propName;
                const GLint propLoc = glGetUniformLocation(shaderID, propCombined.str().c_str());
                if (propLoc < 0) {
                    throw std::invalid_argument("Could not find uniform requested");
                }
                glUniform1i(propLoc, index);
            }
        };

        TerrainPatchGrid_t terrainPatches;
        const double halfPatchTileSize;
        std::vector<std::shared_ptr<TextureGroup>> textures;

        void initTextures(unsigned int shaderID);
    };
}