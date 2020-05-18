//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Mesh.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>

//== IMPLEMENTATION ===========================================================

Mesh::Mesh(std::string _filename)
{
    read_obj(_filename.c_str());
}

bool Mesh::read_obj(const char *_filename)
{
    // open obj file
    std::ifstream ifs(_filename);
    if (!ifs)
    {
        std::cerr << "Can't open " << _filename << "\n";
        return false;
    }

    bool hasNormals = false;
    bool hasUV = false;

    std::string filename(_filename);
    std::string line;
    int counter = -1;
    std::map<int, bool> uvDone;
    // parse line by line
    while (std::getline(ifs, line))
    {
        //devide line into header (first word) and lineData (rest)
        size_t firstSpace = line.find_first_of(" ");
        std::string header = line.substr(0, firstSpace);
        std::istringstream lineData(line.substr(firstSpace + 1));

        //vertices
        if (header == "v")
        {
            Vertex v;
            lineData >> v.position.x >> v.position.y >> v.position.z;

            vertices_.push_back(v);
            continue;
        }

        //uv-coordinates
        if (header == "vt")
        {
            hasUV = true;

            double u, v;

            lineData >> u >> v;

            if (u > 1.0 || u < 0.0)
                u -= floor(u);
            if (v > 1.0 || v < -0.0)
                v -= floor(v);

            u_coordinates_.push_back(u);
            v_coordinates_.push_back(v);
            continue;
        }

        if (header == "vn")
        {
            hasNormals = true;
            continue;
        }

        // material file
        if (header == "mtllib")
        {
            continue;
        }

        // start of new material
        if (header == "usemtl")
        {
            counter++;
            continue;
        }

        // faces
        if (header == "f")
        {
            Triangle t;

            int uv[3];

            enum
            {
                NORMALS,
                UV,
                BOTH,
                NONE
            } nuv_status;
            if (hasUV)
                nuv_status = hasNormals ? BOTH : UV;
            else
                nuv_status = hasNormals ? NORMALS : NONE;

            // dummy varaibles for / and normal indices
            int d1;
            char d2;

            // read in face indices and uv indices, skip normal indices
            switch (nuv_status)
            {
            case BOTH:
                // format: index0/texture0/normal0 index1/texture1/normal1 index2/texture2/normal2
                lineData >> t.i0 >> d2 >> uv[0] >> d2 >> d1;
                lineData >> t.i1 >> d2 >> uv[1] >> d2 >> d1;
                lineData >> t.i2 >> d2 >> uv[2] >> d2 >> d1;

                uv[0]--;
                uv[1]--;
                uv[2]--;
                break;
            case UV:
                // format: index0/texture0 index1/texture1 index2/texture2
                lineData >> t.i0 >> d2 >> uv[0];
                lineData >> t.i1 >> d2 >> uv[1];
                lineData >> t.i2 >> d2 >> uv[2];

                uv[0]--;
                uv[1]--;
                uv[2]--;
            case NORMALS:
                // format: index0//normal0 index1//normal1 index2//normal2
                lineData >> t.i0 >> d2 >> d2 >> d1;
                lineData >> t.i1 >> d2 >> d2 >> d1;
                lineData >> t.i2 >> d2 >> d2 >> d1;
            case NONE:
                // format: index0 index1 index2
                lineData >> t.i0 >> t.i1 >> t.i2;
            }

            //decrement because obj indices start by 1
            t.i0--;
            t.i1--;
            t.i2--;

            // add triangle to our triangle vector
            triangles_.push_back(t);
        }
    }

    std::cout << "\n  read " << _filename << ": " << vertices_.size() << " vertices, " << triangles_.size() << " triangles" << std::flush;

    compute_bounding_box();
    compute_normals();

    return true;
}

//-----------------------------------------------------------------------------

void Mesh::compute_normals()
{
    // compute triangle normals
    for (Triangle &t : triangles_)
    {
        const glm::vec3 &p0 = vertices_[t.i0].position;
        const glm::vec3 &p1 = vertices_[t.i1].position;
        const glm::vec3 &p2 = vertices_[t.i2].position;
        t.normal = normalize(cross(p1 - p0, p2 - p0));
    }

    // initialize vertex normals to zero
    for (Vertex &v : vertices_)
    {
        v.normal = glm::vec3(0, 0, 0);
        glm::vec3 normalSum = v.normal;
        for (Triangle &t : triangles_)
        { // todo: this cannot be the best solution performance wise..
            glm::vec3 vec1, vec2;
            bool match = false;
            if (vertices_[t.i0].position == v.position)
            {
                vec1 = vertices_[t.i1].position - v.position;
                vec2 = vertices_[t.i2].position - v.position;
                match = true;
            }
            else if (vertices_[t.i1].position == v.position)
            {
                vec1 = vertices_[t.i0].position - v.position;
                vec2 = vertices_[t.i2].position - v.position;
                match = true;
            }
            else if (vertices_[t.i2].position == v.position)
            {
                vec1 = vertices_[t.i0].position - v.position;
                vec2 = vertices_[t.i1].position - v.position;
                match = true;
            }
            if (match)
            {
                float angle = glm::dot(vec1, vec2) / (glm::length(vec1) * glm::length(vec2));
                normalSum += (t.normal * std::acos(angle));
            }
        }
        v.normal = normalize(normalSum);
    }
}

bool Mesh::intersect(const Ray &_ray,
                     glm::vec3 &_intersection_point,
                     glm::vec3 &_intersection_normal,
                     glm::vec3 &_intersection_diffuse,
                     double &_intersection_t) const
{
    glm::vec3 p, n, d;
    double t;

    _intersection_t = DBL_MAX;

    // for each triangle
    for (const Triangle &triangle : triangles_)
    {
        // does ray intersect triangle?
        if (intersect_triangle(triangle, _ray, p, n, d, t))
        {
            // is intersection closer than previous intersections?
            if (t < _intersection_t)
            {
                // store data of this intersection
                _intersection_t = t;
                _intersection_point = p;
                _intersection_normal = n;
                _intersection_diffuse = d;
            }
        }
    }

    return (_intersection_t != DBL_MAX);
}

void Mesh::compute_bounding_box()
{
    bb_min_ = glm::vec3(10e10);
    bb_max_ = glm::vec3(-10e10);

    for (Vertex v : vertices_)
    {
        bb_min_ = glm::min(bb_min_, v.position);
        bb_max_ = glm::max(bb_max_, v.position);
        if (v.position.z > 0)
            std::cout << v.position.z << std::endl;
    }
}

//-----------------------------------------------------------------------------

bool Mesh::
    intersect_triangle(const Triangle &_triangle,
                       const Ray &_ray,
                       glm::vec3 &_intersection_point,
                       glm::vec3 &_intersection_normal,
                       glm::vec3 &_intersection_diffuse,
                       double &_intersection_t) const
{
    const Vertex v0 = vertices_[_triangle.i0];
    const Vertex v1 = vertices_[_triangle.i1];
    const Vertex v2 = vertices_[_triangle.i2];
    const glm::vec3 &p0 = v0.position;
    const glm::vec3 &p1 = v1.position;
    const glm::vec3 &p2 = v2.position;

    const glm::vec3 vec_a = p0 - p2;
    const glm::vec3 vec_b = p1 - p2;
    const glm::vec3 vec_t = -_ray.direction;

    // key vector
    const glm::vec3 key = _ray.origin - p2;
    const glm::mat3 matrix = {vec_a, vec_b, vec_t};
    const double determinantBaseMatrix = glm::determinant(matrix);

    // solve for a
    const glm::mat3 matrix_a = {key, vec_b, vec_t};
    const float a = glm::determinant(matrix_a) / (determinantBaseMatrix);
    if (a < 0.0 || a > 1.0)
        return false;

    // solve for b
    const glm::mat3 matrix_b = {vec_a, key, vec_t};
    const float b = glm::determinant(matrix_b) / (determinantBaseMatrix);
    if (b < 0.0 || b > 1.0 || a + b > 1.0)
        return false;

    // solve for t
    const glm::mat3 matrix_t = {vec_a, vec_b, key};
    const float t = glm::determinant(matrix_t) / (determinantBaseMatrix);
    if (t <= 1e-5)
        return false;

    _intersection_t = t;
    _intersection_point = _ray(t);

    return true;
}

//=============================================================================
