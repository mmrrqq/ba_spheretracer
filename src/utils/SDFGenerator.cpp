#include "SDFGenerator.h"

SDFGenerator::SDFGenerator()
{
}

SDFGenerator::SDFGenerator(pmp::SurfaceMesh &mesh, float boxSize, float scaleFactor)
    : texOutput_((int)boxSize * scaleFactor,
                 (int)boxSize * scaleFactor,
                 (int)boxSize * scaleFactor,
                 GL_RGBA32F,
                 GL_RGBA,
                 GL_FLOAT,
                 GL_CLAMP_TO_EDGE,
                 GL_NEAREST),
      data_(std::pow(boxSize * scaleFactor, 3) * 4)
{
    OutX = OutY = OutZ = boxSize * scaleFactor;

    glm::vec3 barycenter(0.0f);
    for (auto v : mesh.vertices())
    {
        pmp::Point vPosition = mesh.position(v);
        barycenter += glm::vec3(vPosition[0], vPosition[1], vPosition[2]);
    }
    barycenter /= mesh.n_vertices();
    glm::vec3 toCenter = glm::vec3(boxSize / 2.0f) - barycenter;

    std::vector<Vertex> vertices(mesh.n_vertices());
    std::vector<Triangle> triangles;

    for (auto v : mesh.vertices())
    {
        pmp::Point vertex = mesh.position(v);
        vertices[v.idx()] = Vertex(
            (vertex[0] + toCenter.x) * scaleFactor,
            (vertex[1] + toCenter.y) * scaleFactor,
            (vertex[2] + toCenter.z) * scaleFactor);
    }

    for (auto f : mesh.faces())
    {
        Triangle t;
        auto fv = mesh.vertices(f).begin();
        t.p0 = (*fv).idx();
        ++fv;
        t.p1 = (*fv).idx();
        ++fv;
        t.p2 = (*fv).idx();
        triangles.push_back(t);
    }

    computeShader_.Load("res/shaders/sdfGenerator.compute");
    computeShader_.Bind();

    computeShader_.SetBuffer(1, sizeof(Vertex) * vertices.size(), vertices.data());
    computeShader_.SetBuffer(2, sizeof(Triangle) * triangles.size(), triangles.data());

    computeShader_.Unbind();
}

void SDFGenerator::Generate(std::vector<float> *data)
{
    computeShader_.Bind();
    texOutput_.BindImage(0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(OutX, OutY, OutZ);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    data_ = std::vector<float>(OutX * OutY * OutZ * 4);
    data_.reserve(OutX * OutY * OutZ * 4);

    float tempData[OutX * OutY * OutZ * 4];

    int tWidth, tHeight, tDepth;

    glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_WIDTH, &tWidth);
    glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_HEIGHT, &tHeight);
    glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_DEPTH, &tDepth);

    std::cout << "width: " << tWidth << " height: " << tHeight << " depth: " << tDepth << std::endl;

    glGetTextureImage(texOutput_.GetID(), 0, GL_RGBA, GL_FLOAT, OutX * OutY * OutZ * 4 * sizeof(float), &tempData[0]);
    computeShader_.Unbind();

    // std::cout << "size of member: " << data_.size() << std::endl;
    // std::cout << "size of data: " << data->size() << std::endl;

    int i;
    for (i = 0; i < data->size(); i++)
    {
        data->data()[i] = tempData[i * 4];
        std::cout << data->data()[i];
    }
}