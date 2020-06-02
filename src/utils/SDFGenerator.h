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
    void Generate(SDField *field);

private:
    unsigned int texInput_, texOutput_, program_, shader_;
    unsigned int loadAndCompile(const char *filename, GLenum type);
    int outX_, outY_, outZ_;
    std::vector<float> data_;
};