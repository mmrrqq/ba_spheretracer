#include "SDField.h"

SDField::SDField() : position_(glm::vec3(0.0, 1.0, 0.0))
{
}

SDField::~SDField()
{
}

void SDField::FromData(const std::vector<float> *data, glm::vec3 size, glm::vec3 position)
{
    position_ = position;
    dimensions_ = size / 150.0f;
    field_ = TextureSampler(
        (int)size.x,
        (int)size.y,
        (int)size.z,
        GL_R32F,
        GL_RED,
        GL_FLOAT,
        (void *)data->data());
}