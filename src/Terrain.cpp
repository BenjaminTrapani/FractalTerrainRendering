//
// Created by Benjamin Trapani on 3/7/18.
//

#include "Terrain.h"

// Generate a patchWidth x patchHeight size set of vertices with triangulations in index buffer.
// Vertices have an origin at x = y = z = 0 and span along x and z from [0, 0] -> [1, 1]
std::shared_ptr<Terrain::VertexIndexBufferData> Terrain::generateVertexIndexBuffers() {
    const std::pair<float, float> spaceBetweenVerts(1.0f / (float)patchWidth, 1.0f / (float)patchHeight);
    // Layout (x y z), (x y z) ...
    auto vertexPositions = std::make_shared<std::vector<float>>(patchWidth * patchHeight * 3);
    unsigned int vertedBufferIndex = 0;
    for (unsigned int z = 0; z < patchHeight; ++z) {
        for (unsigned int x = 0; x < patchWidth; ++x) {
            const float xPos = (float)x * spaceBetweenVerts.first;
            const float zPos = (float)z * spaceBetweenVerts.second;
            vertexPositions->at(vertedBufferIndex++) = xPos;
            vertexPositions->at(vertedBufferIndex++) = zPos;
            vertexPositions->at(vertedBufferIndex++) = 0.0f;
        }
    }

    // Triangulation. Operate on sets of 4 vertices in a square. Triangulate bottom left and then top right.
    auto indices = std::make_shared<std::vector<GLuint>>(indexBufferSize);
    unsigned int indexBufferIndex = 0;
    for (unsigned int z = 0; z < patchHeight - 1; ++z) {
        for (unsigned int x = 0; x < patchWidth - 1; ++x) {
            // Triangle anchored on lower left
            indices->at(indexBufferIndex++) = z * patchWidth + x;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x + 1;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x;

            // Triangle anchored on upper right
            indices->at(indexBufferIndex++) = z * patchWidth + x + 1;
            indices->at(indexBufferIndex++) = (z + 1) * patchWidth + x + 1;
            indices->at(indexBufferIndex++) = z * patchWidth + x;
        }
    }
    return std::make_shared<VertexIndexBufferData>(vertexPositions, indices);
};

void Terrain::GenBuffers() {
    terrainData = generateVertexIndexBuffers();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, terrainData->vertexData->size() * sizeof(terrainData->vertexData->at(0)),
                 terrainData->vertexData->data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainData->indexData->size() * sizeof(terrainData->indexData->at(0)),
                 terrainData->indexData->data(), GL_STATIC_DRAW);
}

Terrain::Terrain(unsigned int ipatchWidth, unsigned int ipatchHeight):
        patchWidth(ipatchWidth), patchHeight(ipatchHeight),
        indexBufferSize(3 * (patchHeight - 1) * (patchWidth - 1) * 2) {
    GenBuffers();
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