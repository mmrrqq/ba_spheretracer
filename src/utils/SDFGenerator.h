#pragma once

#include "SDField.h"
#include "GLUtils.h"
#include "Shader.h"
#include "glm/gtx/component_wise.hpp"
#include "pmp/SurfaceMesh.h"

#include <iostream>
#include <fstream>
#include <sstream>

class SDFGenerator
{
public:
    SDFGenerator();
    SDFGenerator(pmp::SurfaceMesh &mesh);
    ~SDFGenerator();
    SDFGenerator(const SDFGenerator &) = delete;
    SDFGenerator &operator=(const SDFGenerator &) = delete;

    SDFGenerator(SDFGenerator &&other) : program_(other.program_)
    {
        other.program_ = 0;
        other.shader_ = 0;
        other.outX_ = 0;
        other.outY_ = 0;
        other.outZ_ = 0;
        other.texInput_ = TextureSampler();
        other.texOutput_ = TextureSampler();
        other.data_ = std::vector<float>();
    }

    SDFGenerator &operator=(SDFGenerator &&other)
    {
        //ALWAYS check for self-assignment.
        if (this != &other)
        {
            std::swap(program_, other.program_);
            std::swap(shader_, other.shader_);
            std::swap(outX_, other.outX_);
            std::swap(outY_, other.outY_);
            std::swap(outZ_, other.outZ_);
            std::swap(texInput_, other.texInput_);
            std::swap(texOutput_, other.texOutput_);
            std::swap(data_, other.data_);
        }
    }

    void Generate(SDField *field);

private:
    unsigned int program_, shader_;
    TextureSampler texInput_, texOutput_;
    unsigned int loadAndCompile(const char *filename, GLenum type);
    int outX_, outY_, outZ_;
    std::vector<float> data_;
};