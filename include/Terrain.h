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
#include "glad/glad.h"

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
            static constexpr unsigned int slotsPerGroup = 3;

            TextureGroup(const std::string& diffusePath,
                         const std::string& normalPath,
                         const std::string& specularPath,
                         const float iambientFac,
                         const float ishininess,
                         const unsigned int index,
                         const unsigned int shaderID): ambientFac(iambientFac),
                                                       shininess(ishininess),
                                                       slotIndex(index * slotsPerGroup) {
                std::stringstream groupIndexedAccess;
                groupIndexedAccess << "textureGroups[" << index << "]";
                const std::string groupIndexedAccessStr = groupIndexedAccess.str();
                setupUniformNameForSlot(shaderID, groupIndexedAccessStr, "diffuseMap", slotIndex);
                setupUniformNameForSlot(shaderID, groupIndexedAccessStr, "normalMap", slotIndex + 1);
                setupUniformNameForSlot(shaderID, groupIndexedAccessStr, "specularMap", slotIndex + 2);
                diffuseMap.LoadTexture(diffusePath);
                normalMap.LoadTexture(normalPath);
                specularMap.LoadTexture(specularPath);

                std::stringstream ambientStrStream;
                std::stringstream shininessStrStream;
                ambientStrStream << groupIndexedAccessStr << ".ambientFac";
                shininessStrStream << groupIndexedAccessStr << ".shininess";
                ambientFacLoc = glGetUniformLocation(shaderID, ambientStrStream.str().c_str());
                if (ambientFacLoc < 0) {
                    throw std::invalid_argument("Unable to find ambient fac uniform for texture group");
                }
                shininessLoc = glGetUniformLocation(shaderID, shininessStrStream.str().c_str());
                if (shininessLoc < 0) {
                    throw std::invalid_argument("Unable to find shininess uniform for texture group");
                }
            }
            void Bind() {
                diffuseMap.Bind(slotIndex);
                normalMap.Bind(slotIndex + 1);
                specularMap.Bind(slotIndex + 2);

                glUniform1f(ambientFacLoc, ambientFac);
                glUniform1f(shininessLoc, shininess);
            }
        private:
            const float ambientFac;
            const float shininess;
            const unsigned int slotIndex;

            Texture diffuseMap;
            Texture normalMap;
            Texture specularMap;
            GLint ambientFacLoc;
            GLint shininessLoc;

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