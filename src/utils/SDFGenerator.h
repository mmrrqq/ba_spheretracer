#pragma once

#include "GLUtils.h"
#include "raytracing/Mesh.h"

class SDFGenerator
{
public:
    SDFGenerator();
    SDFGenerator(const Mesh &mesh, const int outX, const int outY, const int outZ);
    ~SDFGenerator();
    void Generate();

private:
    unsigned int texInput_, texOutput_, program_, shader_;
    unsigned int loadAndCompile(const char *filename, GLenum type);
    // mesh data
    // texture dimensions
};