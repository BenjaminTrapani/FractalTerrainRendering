//
// Created by Benjamin Trapani on 2/6/18.
//

#include "SimpleCamera.h"
#include "glm/gtc/matrix_transform.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

SimpleCamera::SimpleCamera(float fovDegrees, float aspectRatio):
        yaw((float)M_PI),
        pitch(0.0f),
        projectionMat(glm::perspective(glm::radians(fovDegrees), aspectRatio, 0.1f, 600.0f)){
    updateViewMatAndTransform();
}

void SimpleCamera::updateViewMatAndTransform() {
    // Convert spherical to cartesian coordinates
    lookAtVector = glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
    // rotate to the right 90 degrees
    const float yawRight = yaw - (float)M_PI / 2.0f;
    // construct right vector using rotated yaw in xz plane
    rightVector = glm::vec3(sin(yawRight), 0.0f, cos(yawRight));
    // Cross product of lookAt and right vectors define up vector
    const glm::vec3 upVector = glm::cross(rightVector, lookAtVector);
    const auto viewMat = glm::lookAt(position, position + lookAtVector, upVector);
    combinedTransform = projectionMat * viewMat;
}

void SimpleCamera::updateRotation(float yawOffset, const float pitchOffset) {
    yaw += yawOffset;
    pitch += pitchOffset;

    constexpr float twoPie = (float)M_PI * 2.0f;
    // Don't let camera go upside down
    if (pitch <= -M_PI/2.0) {
        pitch = (float)-M_PI/2.0f + 0.01f;
    }else if (pitch >= M_PI / 2.0) {
        pitch = (float)M_PI / 2.0f - 0.01f;
    }
    yaw = fmodf(yaw, twoPie);

    updateViewMatAndTransform();
}
void SimpleCamera::updateTranslation(const glm::vec2& translationOffset) {
    position += translationOffset.x * rightVector;
    position += translationOffset.y * lookAtVector;
    updateViewMatAndTransform();
}
const glm::mat4& SimpleCamera::getTransform()const {
    return combinedTransform;
}

const glm::vec3& SimpleCamera::getPosition() const {
    return position;
}