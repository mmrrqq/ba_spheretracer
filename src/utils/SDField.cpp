#include "SDField.h"

SDField::SDField(glm::vec3 size)
    : position_(glm::vec3(0.0, 1.0, 0.0)),
      field_((int)size.x,
             (int)size.y,
             (int)size.z,
             GL_R32F,
             GL_RED,
             GL_FLOAT,
             GL_CLAMP_TO_EDGE,
             GL_LINEAR),
      dimensions_(size)
{
}

void SDField::SetData(std::vector<float> *data)
{
    field_.SetData((void *)data->data());
}

void SDField::Scale(float factor)
{
    dimensions_ *= factor;
}