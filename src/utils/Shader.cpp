//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "Shader.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

//=============================================================================

Shader::Shader() : pid_(0), vid_(0), fid_(0), cid_(0) {}

//-----------------------------------------------------------------------------

Shader::~Shader() { Cleanup(); }

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
    locations.clear();
}

//-----------------------------------------------------------------------------

bool Shader::Load(const char *vertexShaderFilePath,
                  const char *fragmentShaderFilePath)
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
        std::cerr << "Shader: Cannot link program:\n" << info << std::endl;
        delete[] info;

        Cleanup();

        return false;
    }

    return true;
}

bool Shader::Load(const char *computeShaderFilePath)
{
    // cleanup existing shaders first
    Cleanup();

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
        std::cerr << "Shader: Cannot link program:\n" << info << std::endl;
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
        std::cerr << "Shader: Cannot create " << type << " shader object for \""
                  << filename << "\"\n";
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

void Shader::Unbind() { glUseProgram(0); }

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

void Shader::SetUniform(const std::string name, TextureSampler &texture,
                        unsigned int slot)
{
    texture.Bind(slot);
    // glUniform1i(getUniformLocation(name), slot);
}

void Shader::SetUniform(const std::string name, SDField *sdField,
                        unsigned int textureSlot)
{
    sdField->Bind(textureSlot);
    glUniform1i(getUniformLocation(name + ".field"), textureSlot);
    glUniform3f(getUniformLocation(name + ".position"), sdField->Position()[0],
                sdField->Position()[1], sdField->Position()[2]);
    glUniform3f(getUniformLocation(name + ".dimensions"),
                sdField->Dimensions()[0], sdField->Dimensions()[1],
                sdField->Dimensions()[2]);
}

// TODO: to Buffer class
// TODO: to named buffer
void Shader::SetBuffer(unsigned int slot, float size, void *data)
{
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ssbo);
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
