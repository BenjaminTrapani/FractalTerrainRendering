//
// Created by Benjamin Trapani on 4/2/18.
//

#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>
#include "glad/glad.h"
#include <memory>

namespace FractalTerrain {
    class Lights {
    public:
        class Light {
            friend class Lights;
        public:
            Light(unsigned int shaderID,
                  unsigned int lightIdx,
                  const glm::vec3 &position,
                  const glm::vec3 &ambientColor,
                  const glm::vec3 &specularColor,
                  const glm::vec3 &diffuseColor);

            void setPosition(const glm::vec3 &position);
            const glm::vec3& getPosition() const {
              return position;
            }
        private:
            void bind();

            static GLint initUniformWithPropName(const std::string& baseUniformName,
                                                 unsigned int shaderID,
                                                 const std::string &propName);

            glm::vec3 position;
            glm::vec3 ambientColor;
            glm::vec3 specularColor;
            glm::vec3 diffuseColor;

            GLint positionLoc;
            GLint ambientColorLoc;
            GLint specularColorLoc;
            GLint diffuseColorLoc;
        };

        Lights(unsigned int shaderID);

        void addLight(const glm::vec3 &position,
                      const glm::vec3 &ambientColor,
                      const glm::vec3 &specularColor,
                      const glm::vec3 &diffuseColor);

        Light &getSun();

        void bind();

    private:
        const unsigned int shaderID;
        std::vector<std::shared_ptr<Light>> lightsToRender;
    };
}
