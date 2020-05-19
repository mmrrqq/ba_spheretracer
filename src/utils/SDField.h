#pragma once

#include "TextureSampler.h"
#include "glm/glm.hpp"
#include <vector>

class SDField
{
public:
    SDField();
    ~SDField();
    void FromData(const std::vector<float> *data, glm::vec3 size);
    inline void Bind(unsigned int slot) { field_.Bind(slot); };
    inline glm::vec3 Dimensions() { return dimensions_; };
    inline glm::vec3 Position() { return position_; };

private:
    TextureSampler field_;
    glm::vec3 position_;
    glm::vec3 dimensions_;
};