//
// Created by Benjamin Trapani on 3/7/18.
//

#pragma once
#include "glad/glad.h"
#include <vector>

class Terrain {

public:
    Terrain(unsigned int patchWidth, unsigned int patchHeight);
    void bindVertexBuffer();
    void bindIndexBuffer();
    GLsizei getIndexBufferCount() const;

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

    std::shared_ptr<VertexIndexBufferData> generateVertexIndexBuffers();
    void GenBuffers();
};
