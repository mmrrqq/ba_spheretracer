#include "SDFGenerator.h"

SDFGenerator::SDFGenerator()
{
}

SDFGenerator::SDFGenerator(pmp::SurfaceMesh &mesh)
{
    pmp::Point bbDim = mesh.bounds().max() - mesh.bounds().min();
    glm::vec3 dimensions = glm::vec3(bbDim[0], bbDim[1], bbDim[2]);

    float bbMaximum = std::ceil(glm::compMax(dimensions));

    outX_ = bbMaximum + 1.0, outY_ = bbMaximum + 1.0, outZ_ = bbMaximum + 1.0;

    float scaleFactor = 15;

    outX_ *= scaleFactor;
    outY_ *= scaleFactor;
    outZ_ *= scaleFactor;

    data_ = std::vector<float>(outZ_ * outY_ * outX_ * 4);

    GLfloat textureData[mesh.n_faces() * 4 * 3];
    // for (int i = 0; i < mesh.n_faces(); i++)
    for (auto f : mesh.faces())
    {
        glm::vec3 toCenter(bbMaximum / 2.0f, bbMaximum / 3.0f, bbMaximum / 2.0f);
        int fvi = 0;
        for (auto fv : mesh.vertices(f))
        {
            pmp::Point vertex = mesh.position(fv);
            glm::vec3 glmVertex = glm::vec3(vertex[0], vertex[1], vertex[2]);
            glmVertex = (glmVertex + toCenter) * scaleFactor;
            textureData[12 * f.idx() + 4 * fvi + 0] = glmVertex.x;
            textureData[12 * f.idx() + 4 * fvi + 1] = glmVertex.y;
            textureData[12 * f.idx() + 4 * fvi + 2] = glmVertex.z;
            textureData[12 * f.idx() + 4 * fvi + 3] = 1.0f; // ALPHA value..
            fvi++;
        }

        // TODO: move object to barycenter of vertices..
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mesh.n_faces() * 3, 1, 0, GL_RGBA, GL_FLOAT, &textureData);
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

    field->FromData(&data_, glm::vec3(outX_, outY_, outZ_), glm::vec3(0.0, 0.5, 0.0));
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