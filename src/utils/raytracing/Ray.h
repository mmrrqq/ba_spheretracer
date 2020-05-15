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

//== CLASS DEFINITION =========================================================

/// \class Ray Ray.h
/// This class implements a ray, specified by its origin and direction.
/// It provides a convenient function to compute the point ray(t) at a specific
/// ray paramter t.
class Ray
{
public:
    /// Constructor with origin and direction. Direction will be normalized.
    Ray(const glm::vec3 &_origin = glm::vec3(0, 0, 0),
        const glm::vec3 &_direction = glm::vec3(0, 0, 1))
    {
        origin = _origin;
        direction = normalize(_direction); // normalize direction
    }

    /// Compute the point on the ray at the parameter \c _t, which is
    /// origin + _t*direction.
    glm::vec3 operator()(float _t) const
    {
        return origin + _t * direction;
    }

public:
    /// origin of the ray
    glm::vec3 origin;
    /// direction of the ray (should be normalized)
    glm::vec3 direction;
};
