//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

#pragma once
//== INCLUDES =================================================================

#include "glm/glm.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <float.h>
#include <string>

//== CLASS DEFINITION =========================================================

/// a vertex consists of a position and a normal
struct Vertex
{
    /// vertex position
    glm::vec3 position;
    /// vertex normal
    glm::vec3 normal;
};

/// a triangle is specified by three indices and a normal
struct Triangle
{
    /// index of first vertex (for array Mesh::vertices_)
    int i0;
    /// index of second vertex (for array Mesh::vertices_)
    int i1;
    /// index of third vertex (for array Mesh::vertices_)
    int i2;
    /// triangle normal
    glm::vec3 normal;
};

/// \class Mesh Mesh.h
/// This class represents a simple triangle mesh, stored as an indexed face set,
/// i.e., as an array of vertices and an array of triangles.
class Mesh
{
public:
    /// Construct a mesh by specifying its draw_mode and its filename
    Mesh(std::string _filename = 0);

private:
    /// Read mesh form an OBJ file
    bool read_obj(const char *_filename);

    /// Compute normal vectors for triangles and vertices
    void compute_normals();

    void compute_bounding_box();

public:
    glm::vec3 bb_min_, bb_max_;
    /// Array of vertices
    std::vector<Vertex>
        vertices_;

    /// Array of triangles
    std::vector<Triangle> triangles_;

    /// u-coordinates to access texture from 0 (left) to 1 (right)
    std::vector<double> u_coordinates_;

    /// v-coordinates to access texture from 0 (bottom) to 1 (top)
    std::vector<double> v_coordinates_;
};

//-----------------------------------------------------------------------------

/// read sphere from stream
inline std::istream &operator>>(std::istream &is, Mesh &m)
{
    std::string filename, mode;
    is >> filename >> mode;

    m = Mesh(filename.c_str());

    return is;
}
