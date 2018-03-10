//
// Created by Benjamin Trapani on 2/6/18.
//

#pragma once

#include "glm/glm.hpp"
#include <utility>

/**
 * Simple camera utility based on math in http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
 */
class SimpleCamera {
public:
    SimpleCamera(float fovDegrees, float aspectRatio);
    // Increment yaw and pitch by given values, constraining pitch such that the camera does not go upside down
    void updateRotation(float yawOffset, float pitchOffset);
    // Move camera by x and y amount where x defines movement along right vector and y defines movement along lookAt vector
    void updateTranslation(const glm::vec2& translationOffset);
    // Get a constant view of the current view projection transform
    const glm::mat4& getTransform() const;
    const glm::vec3& getPosition() const;
private:
    float yaw; //rotation about y in radians
    float pitch; //rotation about x in radians
    const glm::mat4 projectionMat; // constant after creation since it only depends on fov and aspect ratio

    glm::vec3 position;
    glm::mat4 combinedTransform;
    glm::vec3 rightVector;
    glm::vec3 lookAtVector;

    void updateViewMatAndTransform();
};
