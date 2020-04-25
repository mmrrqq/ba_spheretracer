//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#pragma once
//=============================================================================

#include <GL/glew.h>
#include "glm/glm.hpp"
#include <vector>

//=============================================================================

/// shader class for easy handling of the shader
class Shader
{
public:
    /// default constructor
    Shader();
    /// default destructor
    ~Shader();

    /// load (from file), compile, and link shader with given type,
    /// \param file string with the adress to the shader
    /// \param shaderType GLenum for shader type
    bool Load(const char *vertexShader, const char *fragmentShader);

    /// deletes all shader and frees GPU shader capacities
    void Cleanup();

    /// enable/bind this shader program
    void Bind();
    /// disable/unbind this shader program
    void Unbind();

    /// upload bool uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void SetUniform(const char *name, bool value);
    /// upload float uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void SetUniform(const char *name, float value);
    /// upload int uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void SetUniform(const char *name, int value);
    /// upload vec3 uniform
    /// \param name string of the uniform name
    /// \param vec the value for the uniform
    void SetUniform(const char *name, const glm::vec3 &vec);
    /// upload vec4 uniform
    /// \param name string of the uniform name
    /// \param vec the value for the uniform
    void SetUniform(const char *name, const glm::vec4 &vec);
    /// upload mat3 uniform
    /// \param name string of the uniform name
    /// \param mat the value for the uniform
    void SetUniform(const char *name, const glm::mat3 &mat);
    /// upload mat4 uniform
    /// \param name string of the uniform name
    /// \param mat the value for the uniform
    void SetUniform(const char *name, const glm::mat4 &mat);

private:
    /// loads a vertex/fragmend/geometry shader from a file and compiles it
    /// \param filename the location and name of the shader
    /// \param type the type of the shader (vertex, geometry, fragment)
    unsigned int loadAndCompile(const char *filename, GLenum type);

    int getUniformLocation(const char *name);

private:
    /// id of the linked shader program
    unsigned int pid_;
    /// id of the vertex shader
    unsigned int vid_;
    /// id of the fragment shader
    unsigned int fid_;
};