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

Shader::Shader() : pid_(0), vid_(0), fid_(0)
{
}

//-----------------------------------------------------------------------------

Shader::~Shader()
{
    Cleanup();
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

    pid_ = vid_ = fid_ = 0;
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

    // compile vertex shader
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

int Shader::getUniformLocation(const char *name)
{
    if (!pid_)
        return -1;

    int location = glGetUniformLocation(pid_, (const GLchar *)name);
    if (location == -1)
    {
        std::cout << "warning: uniform " << name << " not found" << std::endl;
    }
    return location;
}

//=============================================================================
