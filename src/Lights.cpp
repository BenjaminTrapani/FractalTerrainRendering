//
// Created by Benjamin Trapani on 4/2/18.
//

#include "Lights.h"
#include <sstream>

using namespace FractalTerrain;

Lights::Lights(unsigned int ishaderID, const glm::vec3& iambientLightColor):shaderID(ishaderID),
ambientLightColor(iambientLightColor){
    ambientLightColorLoc = glGetUniformLocation(shaderID, "ambientLightColor");
    if (ambientLightColorLoc < 0) {
        throw std::invalid_argument("Cannot find ambientLightColor uniform");
    }
}

void Lights::addLight(const glm::vec3 &position, const glm::vec3 &ambientColor, const glm::vec3 &specularColor,
                      const glm::vec3 &diffuseColor) {
    lightsToRender.push_back(std::make_shared<Light>(shaderID, lightsToRender.size(), position,
                                                     ambientColor, specularColor, diffuseColor));
}

void Lights::bind() {
    glUniform3fv(ambientLightColorLoc, 1, &ambientLightColor[0]);
    const size_t lightsSize = lightsToRender.size();
    for (unsigned int i = 0; i < lightsSize; ++i) {
        lightsToRender.at(i)->bind();
    }
}

Lights::Light& Lights::getSun() {
    if (lightsToRender.size() == 0) {
        throw std::invalid_argument("At least one light must be added to the scene to fetch the sun. Sun is located at index 0");
    }
    return (*lightsToRender[0]);
}

Lights::Light::Light(const unsigned int shaderID,
                   const unsigned int lightIdx,
                   const glm::vec3& position,
                   const glm::vec3& ambientColor,
                   const glm::vec3& specularColor,
                   const glm::vec3& diffuseColor):position(position),
ambientColor(ambientColor), specularColor(specularColor), diffuseColor(diffuseColor) {
    std::stringstream baseUniformSStream;
    baseUniformSStream << "lights[" << lightIdx << "]";
    const std::string baseUniformName = baseUniformSStream.str();
    ambientColorLoc = Light::initUniformWithPropName(baseUniformName, shaderID, "ambientColor");
    specularColorLoc = Light::initUniformWithPropName(baseUniformName, shaderID, "specularColor");
    diffuseColorLoc = Light::initUniformWithPropName(baseUniformName, shaderID, "diffuseColor");

    std::string positionUniformName;
    if (lightIdx == 0) {
        positionUniformName = "sunDir";
    } else {
        std::stringstream positionNameStream;
        positionNameStream << "lightPositions[" << lightIdx - 1 << "]";
        positionUniformName = positionNameStream.str();
    }

    const GLint tempPositionLoc = glGetUniformLocation(shaderID, positionUniformName.c_str());
    if (tempPositionLoc < 0){
        throw std::invalid_argument("Could not find light position uniform");
    }
    positionLoc = tempPositionLoc;
}

GLint Lights::Light::initUniformWithPropName(const std::string& baseUniformName,
                                             const unsigned int shaderID,
                                            const std::string& propName) {
    std::stringstream propCombinedStream;
    propCombinedStream << baseUniformName << "." << propName;
    const GLint propLoc = glGetUniformLocation(shaderID, propCombinedStream.str().c_str());
    if (propLoc < 0) {
        throw std::invalid_argument("Could not find uniform requested");
    }
    return propLoc;
}

void Lights::Light::setPosition(const glm::vec3& iposition) {
    position = iposition;
}

void Lights::Light::bind() {
    glm::vec3 lightPosForShaders = glm::vec3(position.x, -position.y, position.z);
    glUniform3fv(positionLoc, 1, &lightPosForShaders[0]);
    glUniform3fv(ambientColorLoc, 1, &ambientColor[0]);
    glUniform3fv(specularColorLoc, 1, &specularColor[0]);
    glUniform3fv(diffuseColorLoc, 1, &diffuseColor[0]);
}