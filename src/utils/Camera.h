#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
    Camera();
    ~Camera();
    inline glm::mat4 GetView()
    {
        return glm::lookAt(eye_, lookAt_, up_);
    };
    void Rotate(const double posX, const double posY, const int width, const int height);
    void Reset();
    void Move(const glm::vec3 direction);

private:
    double lastX_, lastY_;
    float yaw_, pitch_;
    glm::vec3 eye_;
    glm::vec3 lookAt_;
    glm::vec3 up_;
};