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

    float scaleFactor = 20;

    outX_ *= scaleFactor;
    outY_ *= scaleFactor;
    outZ_ *= scaleFactor;

    data_ = std::vector<float>(outZ_ * outY_ * outX_ * 4);

    glm::vec3 barycenter(0.0f);
    for (auto v : mesh.vertices())
    {
        pmp::Point vPosition = mesh.position(v);
        barycenter += glm::vec3(vPosition[0], vPosition[1], vPosition[2]);
    }
    barycenter /= mesh.n_vertices();
    glm::vec3 toCenter = glm::vec3(bbMaximum / 2.0f) - barycenter;

    GLfloat textureData[mesh.n_faces() * 4 * 3];
    for (auto f : mesh.faces())
    {
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
    }

    computeShader_.Load("res/shaders/sdfGenerator.compute");
    computeShader_.Bind();

    TextureSampler texInput(
        (int)mesh.n_faces() * 3,
        1,
        GL_RGBA32F,
        GL_RGBA,
        GL_FLOAT,
        GL_CLAMP_TO_EDGE,
        GL_LINEAR,
        &textureData);
    TextureSampler texOutput(
        (int)outX_,
        (int)outY_,
        (int)outZ_,
        GL_RGBA32F,
        GL_RGBA,
        GL_FLOAT,
        nullptr);

    texInput_ = std::move(texInput);
    texOutput_ = std::move(texOutput);

    texInput_.BindImage(0, GL_READ_ONLY, GL_RGBA32F);
    texOutput_.BindImage(1, GL_READ_WRITE, GL_RGBA32F);

    computeShader_.Unbind();
}

SDFGenerator::~SDFGenerator()
{
}

void SDFGenerator::Generate(SDField *field)
{
    computeShader_.Bind();
    glDispatchCompute(outX_, outY_, outZ_);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, &data_[0]);

    field->FromData(&data_, glm::vec3(outX_, outY_, outZ_), glm::vec3(0.0, 0.1, 0.0));
    computeShader_.Unbind();
}