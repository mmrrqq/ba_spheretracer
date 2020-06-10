//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

//=============================================================================

Shader::Shader()
{
}

//-----------------------------------------------------------------------------

Shader::~Shader()
{
    Cleanup();
    locations.~map();
}

//-----------------------------------------------------------------------------

void Shader::Cleanup()
{
    if (pid_)
        glDeleteProgram(pid_);
    if (vid_)
        glDeleteShader(vid_);
    if (fid_)
        glDeleteShader(fid_);
    if (cid_)
        glDeleteShader(cid_);

    pid_ = vid_ = fid_ = cid_ = 0;
}

//-----------------------------------------------------------------------------

bool Shader::Load(const char *vertexShaderFilePath, const char *fragmentShaderFilePath)
{
    // cleanup existing shaders first
    Cleanup();

    // create program
    pid_ = glCreateProgram();

    // vertex shader
    vid_ = loadAndCompile(vertexShaderFilePath, GL_VERTEX_SHADER);
    if (vid_)
        glAttachShader(pid_, vid_);

    // fragment shader
    fid_ = loadAndCompile(fragmentShaderFilePath, GL_FRAGMENT_SHADER);
    if (fid_)
        glAttachShader(pid_, fid_);

    // link program
    glLinkProgram(pid_);
    GLint status;
    glGetProgramiv(pid_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(pid_, GL_INFO_LOG_LENGTH, &length);

        GLchar *info = new GLchar[length + 1];
        glGetProgramInfoLog(pid_, length, NULL, info);
        std::cerr << "Shader: Cannot link program:\n"
                  << info << std::endl;
        delete[] info;

        Cleanup();

        return false;
    }

    return true;
}

bool Shader::Load(const char *computeShaderFilePath)
{
    pid_ = glCreateProgram();
    cid_ = loadAndCompile(computeShaderFilePath, GL_COMPUTE_SHADER);
    glAttachShader(pid_, cid_);

    // link program
    glLinkProgram(pid_);
    GLint status;
    glGetProgramiv(pid_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(pid_, GL_INFO_LOG_LENGTH, &length);

        GLchar *info = new GLchar[length + 1];
        glGetProgramInfoLog(pid_, length, NULL, info);
        std::cerr << "Shader: Cannot link program:\n"
                  << info << std::endl;
        delete[] info;
    }
}

//-----------------------------------------------------------------------------

unsigned int Shader::loadAndCompile(const char *filename, GLenum type)
{
    // read file to string
    std::ifstream ifs(filename);
    if (!ifs)
    {
        std::cerr << "Shader: Cannot open file \"" << filename << "\"\n";
        return 0;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string str = ss.str();
    const char *source = str.c_str();
    ifs.close();

    // create shader
    GLint id = glCreateShader(type);
    if (!id)
    {
        std::cerr << "Shader: Cannot create " << type << " shader object for \"" << filename << "\"\n";
        return 0;
    }

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    // check compile status
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        GLchar *info = new GLchar[length + 1];
        glGetShaderInfoLog(id, length, NULL, info);

        std::cerr << "Shader: Cannot compile shader \"" << filename << "\"\n"
                  << info << std::endl;

        delete[] info;
        glDeleteShader(id);

        return 0;
    }

    return id;
}

//-----------------------------------------------------------------------------

void Shader::Bind()
{
    if (pid_)
        glUseProgram(pid_);
}

//-----------------------------------------------------------------------------

void Shader::Unbind()
{
    glUseProgram(0);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, bool value)
{
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::SetUniform(const char *name, const glm::vec2 &vec)
{
    glUniform2f(getUniformLocation(name), vec[0], vec[1]);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, const glm::vec3 &vec)
{
    glUniform3f(getUniformLocation(name), vec[0], vec[1], vec[2]);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, const glm::vec4 &vec)
{
    glUniform4f(getUniformLocation(name), vec[0], vec[1], vec[2], vec[3]);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(getUniformLocation(name), 1, false, &mat[0][0]);
}

//-----------------------------------------------------------------------------

void Shader::SetUniform(const char *name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, false, &mat[0][0]);
}

void Shader::SetUniform(const std::string name, const PrimitiveScene &scene)
{
    glUniform1i(getUniformLocation(name + ".numSpheres"), scene.NumSpheres);
    glUniform1i(getUniformLocation(name + ".numTori"), scene.NumTori);

    for (int i = 0; i < scene.NumSpheres; i++)
    {
        Sphere sphere = scene.Spheres[i];
        glUniform1i(getUniformLocation(name + ".spheres[" + std::to_string(i) + "].materialId"), sphere.materialId);
        glUniform1f(getUniformLocation(name + ".spheres[" + std::to_string(i) + "].radius"), sphere.radius);
        glUniform3f(
            getUniformLocation(name + ".spheres[" + std::to_string(i) + "].position"),
            sphere.position[0], sphere.position[1], sphere.position[2]);
    }

    for (int i = 0; i < scene.NumTori; i++)
    {
        Torus torus = scene.Tori[i];
        glUniform1i(getUniformLocation(name + ".tori[" + std::to_string(i) + "].materialId"), torus.materialId);
        glUniform1f(getUniformLocation(name + ".tori[" + std::to_string(i) + "].radius"), torus.radius);
        glUniform1f(getUniformLocation(name + ".tori[" + std::to_string(i) + "].tubeRadius"), torus.tubeRadius);
        glUniform3f(
            getUniformLocation(name + ".tori[" + std::to_string(i) + "].position"),
            torus.position[0], torus.position[1], torus.position[2]);
    }
}

void Shader::SetUniform(const std::string name, const SceneLights lights)
{
    glUniform1i(getUniformLocation(name + ".numPointLights"), (int)lights.pointLights.size());
    for (int i = 0; i < lights.pointLights.size(); i++)
    {
        PointLight light = lights.pointLights[i];
        glUniform1f(getUniformLocation(name + ".pointLights[" + std::to_string(i) + "].size"), light.size);
        glUniform3f(
            getUniformLocation(name + ".pointLights[" + std::to_string(i) + "].color"),
            light.color[0], light.color[1], light.color[2]);
        glUniform3f(
            getUniformLocation(name + ".pointLights[" + std::to_string(i) + "].position"),
            light.position[0], light.position[1], light.position[2]);
    }
}

void Shader::SetUniform(const std::string name, const std::vector<Material> &materials)
{
    for (int i = 0; i < materials.size(); i++)
    {
        Material material = materials[i];
        glUniform1f(getUniformLocation(name + "[" + std::to_string(i) + "].shininess"), material.Shininess);
        glUniform3f(
            getUniformLocation(name + "[" + std::to_string(i) + "].ambientColor"),
            material.AmbientColor[0], material.AmbientColor[1], material.AmbientColor[2]);
        glUniform3f(
            getUniformLocation(name + "[" + std::to_string(i) + "].diffuseColor"),
            material.DiffuseColor[0], material.DiffuseColor[1], material.DiffuseColor[2]);
        glUniform3f(
            getUniformLocation(name + "[" + std::to_string(i) + "].specularColor"),
            material.SpecularColor[0], material.SpecularColor[1], material.SpecularColor[2]);
    }
}

void Shader::SetUniform(const std::string name, TextureSampler &texture, unsigned int slot)
{
    texture.Bind(slot);
    glUniform1i(getUniformLocation(name), slot);
}

void Shader::SetUniform(const std::string name, SDField *sdField, unsigned int textureSlot)
{
    sdField->Bind(textureSlot);
    glUniform1i(getUniformLocation(name + ".field"), textureSlot);
    glUniform3f(getUniformLocation(name + ".position"), sdField->Position()[0], sdField->Position()[1], sdField->Position()[2]);
    glUniform3f(getUniformLocation(name + ".dimensions"), sdField->Dimensions()[0], sdField->Dimensions()[1], sdField->Dimensions()[2]);
}

int Shader::getUniformLocation(const std::string name)
{
    if (!pid_)
        return -1;

    auto cachedLocation = locations.find(name);
    if (cachedLocation != locations.end())
    {
        auto location = cachedLocation->second;
        if (location != -1)
            return location;
    }

    int location = glGetUniformLocation(pid_, (const GLchar *)name.c_str());
    if (location == -1)
    {
        std::cout << "warning: uniform " << name << " not found" << std::endl;
    }
    locations[name] = location;
    return location;
}

//=============================================================================
