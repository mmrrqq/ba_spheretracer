#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "SDField.h"
#include "glm/glm.hpp"

// align all structs as 16 byte for ssbo
struct alignas(16) AreaLight
{
    glm::vec4 position;
    glm::vec4 color;
    float size;
};
inline std::istream &operator>>(std::istream &is, AreaLight &l)
{
    is >> l.position[0] >> l.position[1] >> l.position[2] >> l.position[3] >>
        l.color[0] >> l.color[1] >> l.color[2] >> l.color[3] >> l.size;
    return is;
}

struct alignas(16) Sphere
{
    glm::vec4 position;
    float radius;
    int materialId;
};
inline std::istream &operator>>(std::istream &is, Sphere &s)
{
    is >> s.position[0] >> s.position[1] >> s.position[2] >> s.position[3] >>
        s.radius >> s.materialId;
    return is;
}

struct alignas(16) Torus
{
    glm::vec4 position;
    float radius;
    float tubeRadius;
    int materialId;
};
inline std::istream &operator>>(std::istream &is, Torus &t)
{
    is >> t.position[0] >> t.position[1] >> t.position[2] >> t.position[3] >>
        t.radius >> t.tubeRadius >> t.materialId;
    return is;
}

struct alignas(16) Material
{
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    float shininess;
};

inline std::istream &operator>>(std::istream &is, Material &m)
{
    // clang-format off
    is >> m.ambientColor[0] >> m.ambientColor[1] >> m.ambientColor[2] >> m.ambientColor[3] >>   //
        m.diffuseColor[0] >> m.diffuseColor[1] >> m.diffuseColor[2] >> m.diffuseColor[3] >>     //
        m.specularColor[0] >> m.specularColor[1] >> m.specularColor[2] >> m.specularColor[3] >> //
        m.shininess;
    // clang-format on
    return is;
}

/**
 * @brief Class holding scene informations.
 */
class PrimitiveScene
{
public:
    PrimitiveScene() = default;
    ~PrimitiveScene() = default;

    void ReadScene(std::string filePath);

    std::vector<Sphere> Spheres;
    std::vector<Torus> Tori;
    std::vector<Material> Materials;
    std::vector<AreaLight> AreaLights;
    std::vector<SDField> Meshes;
    glm::vec4 Eye, LookAt;

private:
    void clearData();
};