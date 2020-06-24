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

    void release(){};

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

    float getMeshMaxDimension(pmp::SurfaceMesh &mesh);
    glm::vec3 getBarycenter(pmp::SurfaceMesh &mesh);

public:
    enum EBoxSize
    {
        B_32 = 32,
        B_64 = 64,
        B_128 = 128,
        B_256 = 256
    };
    // length of one bounding box side
    EBoxSize BoxSize;
    SDFGenerator();
    SDFGenerator(pmp::SurfaceMesh &mesh, EBoxSize boxSize);
    ~SDFGenerator() { release(); };
    SDFGenerator(const SDFGenerator &) = delete;
    SDFGenerator &operator=(const SDFGenerator &) = delete;

    SDFGenerator(SDFGenerator &&other)
    {
        other.computeShader_ = Shader();
        other.BoxSize = B_32;
        other.texOutput_ = TextureSampler();
        other.triangles_ = std::vector<Triangle>();
        other.vertices_ = std::vector<Vertex>();
    }

    SDFGenerator &operator=(SDFGenerator &&other)
    {
        //ALWAYS check for self-assignment.
        if (this != &other)
        {
            std::swap(computeShader_, other.computeShader_);
            std::swap(BoxSize, other.BoxSize);
            std::swap(texOutput_, other.texOutput_);
            std::swap(triangles_, other.triangles_);
            std::swap(vertices_, other.vertices_);
        }
    }

    TextureSampler *GetOutputTexture() { return &texOutput_; };

    void Dispatch();
};
