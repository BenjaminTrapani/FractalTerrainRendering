//
// Created by Benjamin Trapani on 1/28/18.
//

#pragma once

#include <string>
#include <sstream>
#include <exception>
#include <vector>
#include <fstream>

/**
 * Loads vertex and corresponding index buffers from a .obj file. Assumes that all indexed shapes are triangles.
 */
template<typename VertexParam_t, // Data type of vertex buffer elements
        typename IndexParam_t, // Data type of index buffer elements
        typename VertexConversionFunctor, // A functor that is default constructible and converts a std::tring to a VertexParam_t
        typename IndexConversionFunctor> // A functor that is default constructible and converts a std::string to an IndexParam_t
class OBJFileReader {
public:
    typedef VertexParam_t Vertex_t;
    typedef IndexParam_t Index_t;

    struct OBJFileDataView {
        const std::shared_ptr<std::vector<Vertex_t>> m_vertexData;
        const std::shared_ptr<std::vector<Index_t>> m_indexData;

        OBJFileDataView(std::shared_ptr<std::vector<Vertex_t>> vertexData,
                        std::shared_ptr<std::vector<Index_t>> indexData): m_vertexData(vertexData),
                                                                          m_indexData(indexData) {}
    };

    OBJFileReader(const std::string& objPath): m_objPath(objPath){}

    std::shared_ptr<OBJFileDataView> loadData() {
        std::ifstream objFile(m_objPath);
        if (!objFile.is_open()) {
            std::stringstream errorStream;
            errorStream << "File does not exist at location " << m_objPath;
            throw std::invalid_argument(errorStream.str());
        }

        std::string lineBuf;
        auto vertices = std::make_shared<std::vector<Vertex_t>>();
        auto indices = std::make_shared<std::vector<Index_t>>();

        while(!objFile.eof()) {
            getline(objFile, lineBuf);
            if (lineBuf.size() > 0) {
                switch (lineBuf.at(0)) {
                    case 'v': {
                        if (lineBuf.at(1) == ' ')
                            readVertices(lineBuf, *vertices);
                        break;
                    }
                    case 'f': {
                        readIndices(lineBuf, *indices);
                        break;
                    }
                    default: {
                        //Skip this line because it is a material or vertex normal line
                        break;
                    }
                }
            }
        }

        return std::make_shared<OBJFileDataView>(vertices, indices);
    }

    inline void setObjPath(const std::string& objPath){
        if (objPath.size() == 0)
            throw std::invalid_argument("Path to obj file cannot be empty");
        m_objPath = objPath;
    }

private:
    std::string m_objPath;
    VertexConversionFunctor vertexConversionFunctor;
    IndexConversionFunctor indexConversionFunctor;

    void readVertices(const std::string& vertexLine, std::vector<Vertex_t>& output) {
        //Sample line: v -1.00000000 -1.00000000 -1.00000000
        size_t curReadPos = 2;
        std::string::const_iterator iter;
        while((iter = std::find(vertexLine.begin() + curReadPos, vertexLine.end(), ' '))
              != vertexLine.end()) {
            const size_t endPos = iter - vertexLine.begin();
            const std::string vertexStr = vertexLine.substr(curReadPos, endPos);
            output.push_back(vertexConversionFunctor(vertexStr));
            curReadPos = endPos + 1;
        }
        const std::string vertexStr = vertexLine.substr(curReadPos, vertexLine.size());
        output.push_back(vertexConversionFunctor(vertexStr));
    }

    void readIndices(const std::string& indexLine, std::vector<Index_t>& indices) {
        //Sample line: f 1//1 5//5 2//2
        // The first number before // is the vertex index. The second is the vertex normal index
        size_t beginMarker = 1;
        bool beginMarkerSet = false;
        const size_t indexLineSize = indexLine.size();
        for (size_t i = 1; i < indexLineSize; ++i) {
            const char charHere = indexLine[i];
            switch (charHere) {
                case ' ': {
                    beginMarker = i + 1;
                    beginMarkerSet = true;
                    break;
                }
                case '/': {
                    if (beginMarkerSet) {
                        beginMarkerSet = false;
                        // Read from begin marker to here
                        const std::string vertexIndexStr = indexLine.substr(beginMarker, i);
                        indices.push_back(indexConversionFunctor(vertexIndexStr) - 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
};
