#pragma once

#include "Shader.h"
#include "SDField.h"
#include "GLUtils.h"
#include "glm/gtx/component_wise.hpp"
#include "pmp/SurfaceMesh.h"
#include "pmp/algorithms/TriangleKdTree.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

class SDFGenerator
{
private:
    Shader computeShader_;
    TextureSampler texInput_, texOutput_;
    int outX_, outY_, outZ_;
    std::vector<float> data_;

    struct Triangle
    {
        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec3 p2;
    };

    struct Node
    {
        int leftNode;
        int rightNode;
        std::vector<unsigned int> triangles;
    };

public:
    SDFGenerator();
    SDFGenerator(pmp::SurfaceMesh &mesh);
    ~SDFGenerator();
    SDFGenerator(const SDFGenerator &) = delete;
    SDFGenerator &operator=(const SDFGenerator &) = delete;

    SDFGenerator(SDFGenerator &&other)
    {
        other.computeShader_ = Shader();
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
            std::swap(computeShader_, other.computeShader_);
            std::swap(outX_, other.outX_);
            std::swap(outY_, other.outY_);
            std::swap(outZ_, other.outZ_);
            std::swap(texInput_, other.texInput_);
            std::swap(texOutput_, other.texOutput_);
            std::swap(data_, other.data_);
        }
    }

    void KDTree(pmp::SurfaceMesh &mesh);
    void Generate(SDField *field);
};