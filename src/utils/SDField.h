#pragma once

#include "TextureSampler.h"
#include "glm/glm.hpp"
#include "pmp/SurfaceMesh.h"
#include <vector>

class SDField
{
public:
    SDField();
    SDField(glm::vec3 size, TextureSampler fieldTexture);
    ~SDField() { release(); };

    SDField(const SDField &) = delete;
    SDField &operator=(const SDField &) = delete;

    SDField(SDField &&other)
    {
        other.field_ = TextureSampler();
        other.position_ = glm::vec3(0.0);
        other.dimensions_ = glm::vec3(0.0);
        other.fileName_ = "";
        other.materialId_ = 0;
    }

    SDField &operator=(SDField &&other)
    {
        // ALWAYS check for self-assignment.
        if (this != &other)
        {
            std::swap(field_, other.field_);
            std::swap(position_, other.position_);
            std::swap(dimensions_, other.dimensions_);
            std::swap(fileName_, other.fileName_);
            std::swap(materialId_, other.materialId_);
        }
    }

    void Scale(float factor);
    inline void Bind(unsigned int slot) { field_.Bind(slot); };
    inline void SetFilename(std::string fileName) { fileName_ = fileName; };
    inline void SetPosition(glm::vec3 position) { position_ = position; };
    inline void SetDimensions(glm::vec3 dimensions)
    {
        dimensions_ = dimensions;
    };
    inline void SetMaterialId(int materialId) { materialId_ = materialId; };
    inline void SetField(TextureSampler *texture)
    {
        field_ = std::move(*texture);
    };
    inline void SetUniformName(std::string uniformName)
    {
        uniformName_ = uniformName;
    };
    inline std::string FileName() { return fileName_; };
    inline glm::vec3 Dimensions() { return dimensions_; };
    inline glm::vec3 Position() { return position_; };
    inline int MaterialId() { return materialId_; };
    inline std::string UniformName() { return uniformName_; };

private:
    TextureSampler field_;
    std::string fileName_, uniformName_;
    int materialId_;
    glm::vec3 position_, dimensions_;

    void release() { position_ = dimensions_ = glm::vec3(0.0); };
};

inline std::istream &operator>>(std::istream &is, SDField &f)
{
    std::string fileName, uniformName;
    glm::vec3 position, dimensions;
    int materialId;

    is >> fileName >> position[0] >> position[1] >> position[2] >> //
        dimensions[0] >> dimensions[1] >> dimensions[2] >>         //
        materialId >> uniformName;

    f.SetFilename(fileName);
    f.SetDimensions(dimensions);
    f.SetPosition(position);
    f.SetMaterialId(materialId);
    f.SetUniformName(uniformName);

    return is;
}