#include "Material.h"

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
    AmbientColor = ambient;
    DiffuseColor = diffuse;
    SpecularColor = specular;
    Shininess = shininess;
};

Material::~Material(){};
