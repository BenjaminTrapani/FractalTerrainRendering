//
// Created by Benjamin Trapani on 1/28/18.
//

#pragma once

#include "glad/glad.h"

/**
 * Some default functors compatible with OBJFileReader for OpenGL vertex and index types (float and uint respectively)
 */
template<typename T>
class DefaultOBJReaderFunctor {
public:
    T operator()(const std::string& valueToConvert);
};

template<>
class DefaultOBJReaderFunctor<GLfloat> {
public:
    // In our setup a GLfloat is just a typedef for float, so use atof
    GLfloat operator()(const std::string& valueToConvert) const {
        return atof(valueToConvert.c_str());
    }
};

template<>
class DefaultOBJReaderFunctor<GLuint> {
public:
    // In all setups a GLUInt aliases to the platform's unsigned int type, so convert to unsigned long and cast.
    GLuint operator()(const std::string& valueToConvert) const {
        return (GLuint)std::stoul(valueToConvert);
    }
};
