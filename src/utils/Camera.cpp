#include "Camera.h"

#include <iostream>

Camera::Camera()
    : lastX_(-1),
      lastY_(-1),
      lookAt_(0, 1, 0, 1),
      up_(0, 1, 0, 0),
      eye_(0, 1, 8, 1),
      yaw_(0),
      eyeRoot_(eye_),
      camHeight_(eyeRoot_.y){};
Camera::~Camera(){};

/**
 * @brief Rotate the Camera using the cursor position (posX, posY)
 * Rotates the camera around the lookAt_ member using the yaw_ member.
 * Horizontal Movement is done via the cameraHeight_ member.
 * @param posX Cursors x-axis position.
 * @param posY Cursors x-axis position.
 * @param width Width of the viewport.
 * @param height Height of the viewport.
 */
void Camera::Rotate(const double posX, const double posY, const int width,
                    const int height)
{
    if (lastX_ < 0 || lastY_ < 0)
    {
        lastX_ = posX;
        lastY_ = posY;
        return;
    }

    yaw_ += (posX - lastX_) / (float)height * 5.0;
    camHeight_ += (posY - lastY_) / (float)width * 5.0;

    eye_ = glm::translate(glm::vec3(glm::vec3(0, camHeight_, 0))) *
           glm::translate(glm::vec3(lookAt_)) *
           glm::rotate(yaw_, glm::vec3(0, 1, 0)) *
           glm::translate(glm::vec3(-lookAt_)) * eyeRoot_;

    lastX_ = posX;
    lastY_ = posY;
}

/**
 * @brief Move the Camera by translating eye_, eyeRoot_ and lookAt_.
 * @param direction  Not normalized direction to tranlate with.
 */
void Camera::Move(glm::vec2 direction)
{
    glm::mat4 translation =
        glm::translate(direction.x * glm::normalize(glm::vec3(lookAt_ - eye_)));
    eyeRoot_ = translation * eyeRoot_;
    eye_ = translation * eye_;
    lookAt_ = translation * lookAt_;
}

/**
 * @brief Reset the cursors reference coordinates.
 */
void Camera::Reset()
{
    lastX_ = -1;
    lastY_ = -1;
}