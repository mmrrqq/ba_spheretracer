#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Sphere
{
    int materialId;
    float radius;
    glm::vec3 position;
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
    // return scene instance size in bytes.
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