#include "SDField.h"

SDField::SDField(glm::vec3 size)
    : position_(glm::vec3(0.0, 1.0, 0.0)),
      field_(nullptr),
      dimensions_(size)
{
}

SDField::SDField(glm::vec3 size, TextureSampler *fieldTexture)
    : position_(glm::vec3(0.0, 1.0, 0.0)),
      field_(fieldTexture),
      dimensions_(size)
{
}

void SDField::SetData(std::vector<float> *data)
{
    field_->SetData(data->data());
}

void SDField::Scale(float factor)
{
    dimensions_ *= factor;
}