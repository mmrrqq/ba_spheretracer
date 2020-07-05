#pragma once

#include "GLUtils.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"

class Camera
{
public:
    Camera();
    ~Camera();
    glm::vec4 GetEye()
    {
        return eye_;
    };
    glm::vec4 GetLookAt()
    {
        return lookAt_;
    };
    glm::mat4 GetView()
    {
        return glm::lookAt(glm::vec3(eye_), glm::vec3(lookAt_), glm::vec3(up_));
    };
    void Rotate(const double posX, const double posY, const int width, const int height);
    void Reset();
    void Move(glm::vec2 direciton);

private:
    float yaw_, camHeight_;
    double lastX_, lastY_;
    glm::vec4 eye_;
    glm::vec4 eyeRoot_;
    glm::vec4 lookAt_;
    glm::vec4 up_;
};