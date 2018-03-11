//
// Created by Benjamin Trapani on 3/7/18.
//

#include "TerrainPatch.h"
#include "glm/gtc/matrix_transform.hpp"

// Generate a patchWidth x patchHeight size set of vertices with triangulations in index buffer.
// Vertices have an origin at x = y = z = 0 and span along x and z from [0, 0] -> [1, 1]
std::shared_ptr<TerrainPatch::VertexIndexBufferData> TerrainPatch::generateVertexIndexBuffers() {
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

void TerrainPatch::updateTransform() {
    modelToWorldTransform = glm::translate(glm::mat4(), position);
}

void TerrainPatch::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransform();
}

const glm::mat4& TerrainPatch::getModelToWorldTransform() const {
    return modelToWorldTransform;
}

void TerrainPatch::genBuffers() {
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

TerrainPatch::TerrainPatch(unsigned int ipatchWidth, unsigned int ipatchHeight):
        patchWidth(ipatchWidth), patchHeight(ipatchHeight),
        // Two triangles per square and
        // (width - 1) * (height - 1) squares for a width x height set of vertices.
        // 3 vertices per triangle.
        indexBufferSize(3 * (patchHeight - 1) * (patchWidth - 1) * 2) {
    genBuffers();
    setPosition(glm::vec3());
}

void TerrainPatch::bindVertexBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}
void TerrainPatch::bindIndexBuffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

GLsizei TerrainPatch::getIndexBufferCount() const {
    return indexBufferSize;
}