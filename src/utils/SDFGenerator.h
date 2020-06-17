#pragma once

#include "Shader.h"
#include "SDField.h"
#include "GLUtils.h"
#include "glm/gtx/component_wise.hpp"
#include "pmp/SurfaceMesh.h"
#include "pmp/algorithms/TriangleKdTree.h"
#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

class SDFGenerator
{
private:
    Shader computeShader_;
    TextureSampler texOutput_;
    std::vector<float> data_;

    void release()
    {
        OutX = OutY = OutZ = 0;
    }

    struct Triangle
    {
        int p0, p1, p2;
    };

    struct Vertex
    {
        float x, y, z;
        Vertex(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        };
        Vertex() : x(0), y(0), z(0){};
    };
    std::vector<Triangle> triangles_;
    std::vector<Vertex> vertices_;

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
        other.texOutput_ = TextureSampler();
        other.data_ = std::vector<float>();
        other.triangles_ = std::vector<Triangle>();
        other.vertices_ = std::vector<Vertex>();
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
            std::swap(texOutput_, other.texOutput_);
            std::swap(data_, other.data_);
            std::swap(triangles_, other.triangles_);
            std::swap(vertices_, other.vertices_);
        }
    }

    TextureSampler *GetOutputTexture() { return &texOutput_; }

    void Generate();
};
