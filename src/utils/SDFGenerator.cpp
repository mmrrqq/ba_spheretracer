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
                 GL_LINEAR),
      texTemp_((int)(boxSize * scaleFactor),
               (int)(boxSize * scaleFactor),
               (int)(boxSize * scaleFactor),
               GL_R32F,
               GL_RED,
               GL_FLOAT,
               GL_CLAMP_TO_EDGE,
               GL_NEAREST),
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
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // glPixelStorei(GL_PACK_ALIGNMENT, 1);
    computeShader_.SetBuffer(1, sizeof(Vertex) * vertices_.size(), vertices_.data());
    computeShader_.SetBuffer(2, sizeof(Triangle) * triangles_.size(), triangles_.data());
    copyShader_.Load("res/shaders/sdfCopy.compute");
}

void SDFGenerator::Dispatch()
{
    std::vector<float> depthData(OutX * OutY * OutZ);
    unsigned int dataBuffer;
    glGenBuffers(1, &dataBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * depthData.size(), depthData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dataBuffer);
    texOutput_.BindImage(3, GL_WRITE_ONLY, GL_R32F);

    // texTemp_.BindImage(0, GL_WRITE_ONLY, GL_R32F);

    computeShader_.Bind();
    glDispatchCompute((int)OutX / 16, (int)OutY / 16, (int)OutZ / 4);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    copyShader_.Bind();
    glDispatchCompute((int)OutX / 16, (int)OutY / 16, (int)OutZ / 4);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    copyShader_.Unbind();
}
