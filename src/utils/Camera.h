#pragma once

#include "GLUtils.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"

/**
 * @brief Camera class.
 * Uses the glm lib to generate the View Matrix and to manipulate the camera
 * coordinates.
 */
class Camera
{
public:
    Camera();
    ~Camera();
    inline glm::vec4 Eye() { return eye_; };
    inline glm::vec4 LookAt() { return lookAt_; };
    inline glm::mat4 View()
    {
        return glm::lookAt(glm::vec3(eye_), glm::vec3(lookAt_), glm::vec3(up_));
    };
    inline void SetEye(glm::vec4 eye)
    {
        eye_ = eye;
        eyeRoot_ = eye;
    };
    inline void SetLookAt(glm::vec4 lookAt) { lookAt_ = lookAt; };

    void Rotate(const double posX, const double posY, const int width,
                const int height);
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