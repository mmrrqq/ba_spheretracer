#pragma once

#include "TextureSampler.h"
#include "glm/glm.hpp"
#include <vector>

class SDField
{
public:
    SDField(glm::vec3 size);
    SDField(glm::vec3 size, TextureSampler *fieldTexture);
    ~SDField() { release(); };

    SDField(const SDField &) = delete;
    SDField &operator=(const SDField &) = delete;

    SDField(SDField &&other)
    {
        other.field_ = nullptr;
        other.position_ = glm::vec3(0.0);
        other.dimensions_ = glm::vec3(0.0);
    }

    SDField &operator=(SDField &&other)
    {
        //ALWAYS check for self-assignment.
        if (this != &other)
        {
            std::swap(field_, other.field_);
            std::swap(position_, other.position_);
            std::swap(dimensions_, other.dimensions_);
        }
    }

    void SetData(std::vector<float> *data);
    void Scale(float factor);
    inline void Bind(unsigned int slot) { field_->Bind(slot); };
    inline glm::vec3 Dimensions() { return dimensions_; };
    inline glm::vec3 Position() { return position_; };

private:
    TextureSampler *field_;
    glm::vec3 position_;
    glm::vec3 dimensions_;

    void release()
    {
        position_ = dimensions_ = glm::vec3(0.0);
        field_ = nullptr;
    };
};