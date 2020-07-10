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

#include <map>
#include <string>
#include <vector>

#include "GLUtils.h"
#include "PrimitiveScene.h"
#include "SDField.h"
#include "TextureSampler.h"
#include "glm/glm.hpp"

//=============================================================================

/**
 * @brief Shader class.
 * Can hold vertex+fragment shader or compute shader.
 */
class Shader
{
public:
    /// default constructor
    Shader();
    /// default destructor
    ~Shader();

    /// load (from file), compile, and link shader with given type,
    /// \param vertexShaderFilePath string with the adress to the shader
    /// \param fragmentShaderFilePath string with the adress to the shader
    bool Load(const char *vertexShaderFilePath,
              const char *fragmentShaderFilePath);
    bool Load(const char *computeShaderFilePath);

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
    /// upload vec2 uniform
    /// \param name string of the uniform name
    /// \param vec the value for the uniform
    void SetUniform(const char *name, const glm::vec2 &vec);
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

    /// upload Materials vector uniform
    /// \param name string of the uniform name
    /// \param materials the value for the uniform
    void SetUniform(const std::string, const std::vector<Material> &materials);

    /// upload texture sampler
    /// \param name string of the uniform name
    /// \param texture the value for the uniform
    void SetUniform(const std::string, TextureSampler &texture,
                    unsigned int slot);

    /// upload sd field struct uniform
    /// \param name string of the uniform name
    /// \param sdField the value for the uniform
    void SetUniform(const std::string name, SDField *sdField,
                    unsigned int textureSlot);

    /// set ssbo
    void SetBuffer(unsigned int slot, float size, void *data);

private:
    /// loads a vertex/fragmend/geometry shader from a file and compiles it
    /// \param filename the location and name of the shader
    /// \param type the type of the shader (vertex, geometry, fragment)
    unsigned int loadAndCompile(const char *filename, GLenum type);

    int getUniformLocation(std::string name);

    /// deletes all shader and frees GPU shader capacities
    void cleanup();

    unsigned int pid_, vid_, fid_, cid_;

    std::map<std::string, int> locations;
};
