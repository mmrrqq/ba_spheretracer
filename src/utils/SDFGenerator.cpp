#include "SDFGenerator.h"
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

SDFGenerator::SDFGenerator()
{
}

SDFGenerator::SDFGenerator(const Mesh &mesh)
{
    // TODO: move object to barycenter of vertices..
    glm::vec3 dimensions = mesh.bb_max_ - mesh.bb_min_;

    outX_ = std::ceil(dimensions.x) + 1.0, outY_ = std::ceil(dimensions.y) + 1.0, outZ_ = std::ceil(dimensions.z) + 1.0;

    float scaleFactor = 20;

    outX_ *= scaleFactor;
    outY_ *= scaleFactor;
    outZ_ *= scaleFactor;

    data_ = std::vector<float>(outZ_ * outY_ * outX_ * 4);

    // TODO: generate float array/buffer from mesh triangles..
    GLfloat textureData[mesh.triangles_.size() * 4 * 3];
    for (int i = 0; i < mesh.triangles_.size(); i++)
    {
        Triangle triangle = mesh.triangles_[i];
        Vertex v1 = mesh.vertices_[triangle.i0];
        Vertex v2 = mesh.vertices_[triangle.i1];
        Vertex v3 = mesh.vertices_[triangle.i2];

        v1.position *= scaleFactor;
        v2.position *= scaleFactor;
        v3.position *= scaleFactor;

        textureData[12 * i + 0] = v1.position.x + outX_ / 2.0;
        textureData[12 * i + 1] = v1.position.y + outY_ / 2.0;
        textureData[12 * i + 2] = v1.position.z + outZ_ / 2.0;
        textureData[12 * i + 3] = 1.0f; // ALPHA value..

        textureData[12 * i + 4] = v2.position.x + outX_ / 2.0;
        textureData[12 * i + 5] = v2.position.y + outY_ / 2.0;
        textureData[12 * i + 6] = v2.position.z + outZ_ / 2.0;
        textureData[12 * i + 7] = 1.0f; // ALPHA value..

        textureData[12 * i + 8] = v3.position.x + outX_ / 2.0;
        textureData[12 * i + 9] = v3.position.y + outY_ / 2.0;
        textureData[12 * i + 10] = v3.position.z + outZ_ / 2.0;
        textureData[12 * i + 11] = 1.0f; // ALPHA value..
    }

    // create program
    program_ = glCreateProgram();

    shader_ = loadAndCompile("res/shaders/sdfGenerator.compute", GL_COMPUTE_SHADER);
    glAttachShader(program_, shader_);

    // link program
    glLinkProgram(program_);
    GLint status;
    glGetProgramiv(program_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

        GLchar *info = new GLchar[length + 1];
        glGetProgramInfoLog(program_, length, NULL, info);
        std::cerr << "Shader: Cannot link program:\n"
                  << info << std::endl;
        delete[] info;
    }

    glGenTextures(1, &texInput_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texInput_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mesh.triangles_.size() * 3, 1, 0, GL_RGBA, GL_FLOAT, &textureData);
    glBindImageTexture(0, texInput_, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &texOutput_);
    glBindTexture(GL_TEXTURE_3D, texOutput_);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, outX_, outY_, outZ_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(1, texOutput_, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glUseProgram(0);
}

SDFGenerator::~SDFGenerator()
{
}

void SDFGenerator::Generate(SDField *field)
{
    glUseProgram(program_);
    glDispatchCompute(outX_, outY_, outZ_);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, &data_[0]);

    field->FromData(&data_, glm::vec3(outX_, outY_, outZ_));
}

unsigned int SDFGenerator::loadAndCompile(const char *filename, GLenum type)
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