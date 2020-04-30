#pragma once

#include "glm/glm.hpp"

class Material
{
public:
    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
    ~Material();
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float Shininess;
};