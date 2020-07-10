#include "SDField.h"

SDField::SDField()
{
}

SDField::SDField(glm::vec3 size, TextureSampler fieldTexture)
    : position_(glm::vec3(0.0, 2.0, 0.0)),
      field_(std::move(fieldTexture)),
      dimensions_(size)
{
}

void SDField::Scale(float factor)
{
    dimensions_ *= factor;
}