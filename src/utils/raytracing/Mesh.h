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

#include "Ray.h"
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

    /// Intersect mesh with ray (calls ray-triangle intersection)
    /// If \c _ray intersects a face of the mesh, it provides the following results:
    /// \param[in] _ray the ray to intersect the mesh with
    /// \param[out] _intersection_point the point of intersection
    /// \param[out] _intersection_normal the surface normal at intersection point
    /// \param[out] _intersection_t ray parameter at the intersection point
    virtual bool intersect(const Ray &_ray,
                           glm::vec3 &_intersection_point,
                           glm::vec3 &_intersection_normal,
                           glm::vec3 &_intersection_diffuse,
                           double &_intersection_t) const;

private:
    /// Read mesh form an OBJ file
    bool read_obj(const char *_filename);

    /// Compute normal vectors for triangles and vertices
    void compute_normals();

    /// Intersect a triangle with a ray. Return whether there is an intersection.
    /// If there is an intersection, store intersection data.
    /// This function overrides Object::intersect().
    /// \param[in] _triangle the triangle to be intersected
    /// \param[in] _ray the ray to intersect the triangle with
    /// \param[out] _intersection_point the point of intersection
    /// \param[out] _intersection_normal the surface normal at intersection point
    /// \param[out] _intersection_t ray parameter at the intersection point
    bool intersect_triangle(const Triangle &_triangle,
                            const Ray &_ray,
                            glm::vec3 &_intersection_point,
                            glm::vec3 &_intersection_normal,
                            glm::vec3 &_intersection_diffuse,
                            double &_intersection_t) const;

public:
    /// Array of vertices
    std::vector<Vertex> vertices_;

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
