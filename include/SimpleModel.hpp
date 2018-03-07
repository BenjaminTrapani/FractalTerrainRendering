//
// Created by Benjamin Trapani on 1/29/18.
//

#pragma once
#include "glad/glad.h"

template<typename OBJFileReader_t>
class SimpleModel {
public:
    SimpleModel(std::shared_ptr<typename OBJFileReader_t::OBJFileDataView> data):m_data(data){
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data->m_vertexData->size() * sizeof(typename OBJFileReader_t::Vertex_t),
                     data->m_vertexData->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->m_indexData->size() * sizeof(typename OBJFileReader_t::Index_t),
                     data->m_indexData->data(), GL_STATIC_DRAW);
    }
    ~SimpleModel() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }
    void bindVertexBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }
    void bindIndexBuffer() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }
    GLsizei getIndexBufferCount() {
        return (GLsizei)m_data->m_indexData->size();
    }
private:
    std::shared_ptr<typename OBJFileReader_t::OBJFileDataView> m_data;
    GLuint vbo;
    GLuint ibo;
};
