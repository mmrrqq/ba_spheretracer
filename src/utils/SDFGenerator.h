#pragma once

#include "GLUtils.h"
#include "raytracing/Mesh.h"

class SDFGenerator
{
public:
    SDFGenerator();
    SDFGenerator(const Mesh &mesh);
    ~SDFGenerator();
    void Generate();

private:
    unsigned int texInput_, texOutput_, program_, shader_;
    unsigned int loadAndCompile(const char *filename, GLenum type);
    int outX_, outY_, outZ_;
    std::vector<float> data_;
    // mesh data
    // texture dimensions
};