#include "Camera.h"
#include <iostream>

Camera::Camera()
    : lastX_(-1),
      lastY_(-1),
      lookAt_(0, 0, 0, 1),
      up_(0, 1, 0, 0),
      eye_(0, 1, 4, 1),
      yaw_(0),
      camHeight_(eyeRoot_.y),
      eyeRoot_(1.5, 1, 4, 1){};
Camera::~Camera(){};

void Camera::Rotate(const double posX, const double posY, const int width, const int height)
{
    if (lastX_ < 0 || lastY_ < 0)
    {
        lastX_ = posX;
        lastY_ = posY;
        return;
    }

    yaw_ += (posX - lastX_) / (float)height * 5.0;
    camHeight_ += (posY - lastY_) / (float)width * 5.0;

    eye_ = glm::translate(glm::vec3(glm::vec3(0, camHeight_, 0))) * glm::translate(glm::vec3(lookAt_)) * glm::rotate(yaw_, glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-lookAt_)) * eyeRoot_;

    lastX_ = posX;
    lastY_ = posY;
}

void Camera::Move(glm::vec2 direction)
{
    // vec2(x > 0 ? w : s, y > 0 ? d : a)
    glm::mat4 translation = glm::translate(direction.x * glm::normalize(glm::vec3(lookAt_ - eye_)));
    eyeRoot_ = translation * eyeRoot_;
    eye_ = translation * eye_;
    lookAt_ = translation * lookAt_;
}

void Camera::Reset()
{
    lastX_ = -1;
    lastY_ = -1;
}