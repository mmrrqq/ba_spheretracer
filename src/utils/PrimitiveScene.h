#pragma once

#include "glm/glm.hpp"
#include <vector>

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
    float size;
};

struct SceneLights
{
    std::vector<PointLight> pointLights;
};

struct alignas(16) Sphere
{
    glm::vec3 position;
    float radius;
    int materialId;
};

struct Torus
{
    int materialId;
    glm::vec3 position;
    float radius;
    float tubeRadius;
};

class PrimitiveScene
{
public:
    PrimitiveScene();
    ~PrimitiveScene();
    inline void AddSphere(Sphere sphere)
    {
        Spheres.push_back(sphere);
        NumSpheres++;
    };
    inline void AddTorus(Torus torus)
    {
        Tori.push_back(torus);
        NumTori++;
    };
    int NumSpheres;
    int NumTori;
    std::vector<Sphere> Spheres;
    std::vector<Torus> Tori;
};