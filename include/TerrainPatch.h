//
// Created by Benjamin Trapani on 3/7/18.
//

#pragma once
#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>

class TerrainPatch {
friend class Terrain;
public:
    TerrainPatch(unsigned int patchWidth, unsigned int patchHeight);
    void bindVertexBuffer();
    void bindIndexBuffer();
    GLsizei getIndexBufferCount() const;
    void setPosition(const glm::vec3& pos);

    // Here so it can be inlined. It is called frequently in Terrain
    const glm::vec3& getPosition() const {
        return position;
    }

    const glm::mat4& getModelToWorldTransform() const;
private:
    struct VertexIndexBufferData {
        std::shared_ptr<std::vector<float>> vertexData;
        std::shared_ptr<std::vector<GLuint>> indexData;
        VertexIndexBufferData(std::shared_ptr<std::vector<float>> vData,
                              std::shared_ptr<std::vector<GLuint>> iData): vertexData(vData), indexData(iData){}
    };

    const unsigned int patchWidth;
    const unsigned int patchHeight;
    const unsigned int indexBufferSize;
    GLuint vbo;
    GLuint ibo;
    glm::vec3 position;
    glm::mat4 modelToWorldTransform;

    std::shared_ptr<VertexIndexBufferData> generateVertexIndexBuffers();
    void genBuffers();
    void updateTransform();
};
