#pragma once

#include "glm/glm.hpp"
#include <vector>

// align all structs as 16 byte for ssbo
struct alignas(16) AreaLight
{
    glm::vec4 position;
    glm::vec4 color;
    float size;
};

struct alignas(16) Sphere
{
    glm::vec4 position;
    float radius;
    int materialId;
};

struct alignas(16) Torus
{
    glm::vec4 position;
    float radius;
    float tubeRadius;
    int materialId;
};

struct alignas(16) Material
{
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    float shininess;
};

class PrimitiveScene
{
public:
    PrimitiveScene() = default;
    ~PrimitiveScene() = default;

    std::vector<Sphere> Spheres;
    std::vector<Torus> Tori;
    std::vector<Material> Materials;
    std::vector<AreaLight> AreaLights;
};