#pragma once

#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "GLUtils.h"
#include "SDField.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/component_wise.hpp"
#include "pmp/SurfaceMesh.h"
#include "pmp/algorithms/TriangleKdTree.h"

/**
 * @brief Class to calculate signed distance fields from meshes.
 * Holds and controls the compute shader for generating distance fields from
 * meshes. Does all the computation to fill the texture properly.
 */
class SDFGenerator
{
public:
    enum EBoxSize
    {
        B_32 = 32,
        B_64 = 64,
        B_128 = 128,
        B_256 = 256
    };
    // length of one bounding box side
    SDFGenerator();
    SDFGenerator(pmp::SurfaceMesh &mesh, EBoxSize boxSize);
    ~SDFGenerator() { release(); };
    SDFGenerator(const SDFGenerator &) = delete;
    SDFGenerator &operator=(const SDFGenerator &) = delete;

    SDFGenerator(SDFGenerator &&other)
    {
        other.computeShader_ = Shader();
        other.boxSize_ = B_32;
        other.texOutput_ = TextureSampler();
        other.triangles_ = std::vector<Triangle>();
        other.vertices_ = std::vector<Vertex>();
    }

    SDFGenerator &operator=(SDFGenerator &&other)
    {
        // ALWAYS check for self-assignment.
        if (this != &other)
        {
            std::swap(computeShader_, other.computeShader_);
            std::swap(boxSize_, other.boxSize_);
            std::swap(texOutput_, other.texOutput_);
            std::swap(triangles_, other.triangles_);
            std::swap(vertices_, other.vertices_);
        }
    }

    TextureSampler *OutputTexture() { return &texOutput_; };

    void Dispatch();

private:
    Shader computeShader_;
    TextureSampler texOutput_;
    EBoxSize boxSize_;

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
};
