//
// Created by Benjamin Trapani on 3/7/18.
//

#include "TerrainPatch.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

void TerrainPatch::updateTransform() {
    const glm::mat4 translation = glm::translate(position);
    const glm::mat4 scaleMat = glm::scale(glm::vec3(scale, scale, scale));
    modelToWorldTransform = translation * scaleMat;
}

void TerrainPatch::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransform();
}

const glm::mat4& TerrainPatch::getModelToWorldTransform() const {
    return modelToWorldTransform;
}

TerrainPatch::TerrainPatch(unsigned int ipatchWidth, unsigned int ipatchHeight, 
                           const glm::vec3& position, float scl):
        patchWidth(ipatchWidth), patchHeight(ipatchHeight) {
    scale = scl;
    setPosition(position);
}