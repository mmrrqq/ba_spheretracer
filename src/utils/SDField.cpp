#include "SDField.h"

SDField::SDField() : position_(glm::vec3(0.0))
{
}

SDField::~SDField()
{
}

void SDField::FromData(const std::vector<float> *data, glm::vec3 size)
{
    dimensions_ = size / 40.0f;
    field_ = TextureSampler(
        (int)size.x,
        (int)size.y,
        (int)size.z,
        GL_RGBA32F,
        GL_RGBA,
        GL_FLOAT,
        (void *)data->data());
}