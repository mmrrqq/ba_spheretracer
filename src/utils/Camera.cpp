#include "Camera.h"

Camera::Camera()
    : lastX_(-1),
      lastY_(-1),
      pitch_(0),
      yaw_(0),
      lookAt_(0.0, 0.0, -5.0),
      up_(0.0, 1.0, 0.0),
      eye_(0.0, 0.0, 0.0){};
Camera::~Camera(){};

void Camera::Rotate(const double posX, const double posY, const int width, const int height)
{
    if (lastX_ < 0 || lastY_ < 0)
    {
        lastX_ = posX;
        lastY_ = posY;
        return;
    }

    yaw_ += (posX - lastX_) / (float)height * 100.0;
    pitch_ += (lastY_ - posY) / (float)width * 100.0;

    // learnopengl.com - camera
    lookAt_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    lookAt_.y = sin(glm::radians(pitch_));
    lookAt_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    lookAt_ = glm::normalize(lookAt_);

    lastX_ = posX;
    lastY_ = posY;
}

void Camera::Move(const glm::vec3 direction)
{
}

void Camera::Reset()
{
    lastX_ = -1;
    lastY_ = -1;
}