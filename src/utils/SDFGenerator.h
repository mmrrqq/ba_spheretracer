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
    std::vector<float> data_;

    void release()
    {
        computeShader_.~Shader();
        OutX = OutY = OutZ = 0;
        texInput_.~TextureSampler();
        texOutput_.~TextureSampler();
        data_.~vector();
    }

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
    int OutX, OutY, OutZ;
    SDFGenerator();
    SDFGenerator(pmp::SurfaceMesh &mesh, float boxSize, float scaleFactor);
    ~SDFGenerator() { release(); };
    SDFGenerator(const SDFGenerator &) = delete;
    SDFGenerator &operator=(const SDFGenerator &) = delete;

    SDFGenerator(SDFGenerator &&other)
    {
        other.computeShader_ = Shader();
        other.OutX = 0;
        other.OutY = 0;
        other.OutZ = 0;
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
            std::swap(OutX, other.OutX);
            std::swap(OutY, other.OutY);
            std::swap(OutZ, other.OutZ);
            std::swap(texInput_, other.texInput_);
            std::swap(texOutput_, other.texOutput_);
            std::swap(data_, other.data_);
        }
    }

    void KDTree(pmp::SurfaceMesh &mesh);
    std::vector<float> Generate();
};