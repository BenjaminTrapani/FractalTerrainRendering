//
// Created by Benjamin Trapani on 3/7/18.
//

#pragma once
#include "OpenGLDefs.h"
#include <vector>
#include <glm/glm.hpp>

class TerrainPatch {
friend class Terrain;
public:
    TerrainPatch(unsigned int patchWidth, unsigned int patchHeight,
                  const glm::vec3& position, float scale);
    void setPosition(const glm::vec3& pos);

    // Here so it can be inlined. It is called frequently in Terrain
    const glm::vec3& getPosition() const {
        return position;
    }

    const glm::mat4& getModelToWorldTransform() const;
private:
    glm::vec3 position;
    glm::mat4 modelToWorldTransform;
    float scale;
    unsigned int patchWidth;
    unsigned int patchHeight;

    void updateTransform();
};
