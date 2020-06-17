#include "SDFGenerator.h"

SDFGenerator::SDFGenerator()
{
}

SDFGenerator::SDFGenerator(pmp::SurfaceMesh &mesh, float boxSize, float scaleFactor)
    : texOutput_((int)(boxSize * scaleFactor),
                 (int)(boxSize * scaleFactor),
                 (int)(boxSize * scaleFactor),
                 GL_R32F,
                 GL_RED,
                 GL_FLOAT,
                 GL_CLAMP_TO_EDGE,
                 GL_NEAREST),
      data_(std::pow((int)(boxSize * scaleFactor), 3)),
      vertices_(mesh.n_vertices()),
      triangles_(mesh.n_faces())
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

    for (auto v : mesh.vertices())
    {
        pmp::Point vertex = mesh.position(v);
        vertices_.at(v.idx()) = Vertex(
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
        triangles_.at(f.idx()) = t;
    }

    std::cout << mesh.n_faces() << " t_size: " << triangles_.size() << std::endl;
    std::cout << mesh.n_vertices() << " v_size: " << vertices_.size() << std::endl;

    computeShader_.Load("res/shaders/sdfGenerator.compute");
    computeShader_.Bind();

    texOutput_.BindImage(0, GL_WRITE_ONLY, GL_R32F);
    computeShader_.SetBuffer(1, sizeof(Vertex) * vertices_.size(), vertices_.data());
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    computeShader_.SetBuffer(2, sizeof(Triangle) * triangles_.size(), triangles_.data());
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // glPixelStorei(GL_PACK_ALIGNMENT, 1);

    computeShader_.Unbind();
}

void SDFGenerator::Generate()
{
    computeShader_.Bind();

    glDispatchCompute((int)OutX / 16, (int)OutY / 16, (int)OutZ / 4);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // int tWidth, tHeight, tDepth, tAlignment, tIFormat;

    // glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_WIDTH, &tWidth);
    // glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_HEIGHT, &tHeight);
    // glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_DEPTH, &tDepth);
    // glGetTextureLevelParameteriv(texOutput_.GetID(), 0, GL_TEXTURE_INTERNAL_FORMAT, &tIFormat);

    // glGetIntegerv(GL_PACK_ALIGNMENT, &tAlignment);

    // std::cout << "width: " << tWidth << " height: " << tHeight << " depth: " << tDepth << " alignment: " << tAlignment << std::endl;
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    // glGetTextureImage(texOutput_.GetID(), 0, GL_RGBA, GL_FLOAT, OutX * OutY * OutZ * 4 * sizeof(float), data_.data());
    //    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, data_.data());
    computeShader_.Unbind();

    // std::cout << "size of member: " << data_.size() << std::endl;
    // std::cout << "size of data: " << data->size() << std::endl;

    // int i;
    // for (i = 0; i < data->size(); i++)
    // {
    //     data->data()[i] = data_.data()[i * 4];
    // }
    // std::cout << "i: " << i << data->data()[i] << " " << data->data()[i - 1] << std::endl;
}
