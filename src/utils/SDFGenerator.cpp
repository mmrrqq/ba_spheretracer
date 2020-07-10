#include "SDFGenerator.h"

SDFGenerator::SDFGenerator() {}

/**
 * @brief Construct a new SDFGenerator::SDFGenerator object.
 * This sets up the output texture and moves/scales the mesh vertices to fill
 * out the box properly. Then loads the compute shader to be ready for
 * dispatching.
 * @param mesh pmp::SurfaceMesh to be processed.
 * @param boxSize Enum of powers of two to set the 3D texture dimensions.
 */
SDFGenerator::SDFGenerator(pmp::SurfaceMesh &mesh, EBoxSize boxSize)
    : texOutput_(boxSize, boxSize, boxSize, GL_R32F, GL_RED, GL_FLOAT,
                 GL_CLAMP_TO_EDGE, GL_LINEAR),
      vertices_(mesh.n_vertices()),
      triangles_(mesh.n_faces()),
      boxSize_(boxSize)
{
    float maxMeshDim = getMeshMaxDimension(mesh);
    float scaleFactor = boxSize_ / maxMeshDim - 0.1;  // ten percent border

    glm::vec3 barycenter = getBarycenter(mesh);

    for (auto v : mesh.vertices())
    {
        pmp::Point vertex = mesh.position(v);
        vertices_.at(v.idx()) =
            Vertex((vertex[0] - barycenter.x) * scaleFactor + boxSize_ / 2.0,
                   (vertex[1] - barycenter.y) * scaleFactor + boxSize_ / 2.0,
                   (vertex[2] - barycenter.z) * scaleFactor + boxSize_ / 2.0);
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

    computeShader_.Load("res/shaders/sdfGenerator.compute");
}

/**
 * @brief Dispatches the compute shader instances.
 * Requires a local compute shader size of 16 for x and y and 4 for z.
 */
void SDFGenerator::Dispatch()
{
    computeShader_.Bind();
    computeShader_.SetBuffer(1, sizeof(Vertex) * vertices_.size(),
                             vertices_.data());
    computeShader_.SetBuffer(2, sizeof(Triangle) * triangles_.size(),
                             triangles_.data());
    texOutput_.BindImage(0, GL_WRITE_ONLY, GL_R32F);

    glDispatchCompute(boxSize_ / 16, boxSize_ / 16, boxSize_ / 4);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);  // TODO: which barrier?

    computeShader_.Unbind();
}

float SDFGenerator::getMeshMaxDimension(pmp::SurfaceMesh &mesh)
{
    pmp::Point bbDim = mesh.bounds().max() - mesh.bounds().min();
    glm::vec3 dimensions = glm::vec3(bbDim[0], bbDim[1], bbDim[2]);
    return std::ceil(glm::compMax(dimensions));
}

glm::vec3 SDFGenerator::getBarycenter(pmp::SurfaceMesh &mesh)
{
    glm::vec3 barycenter(0.0f);
    for (auto v : mesh.vertices())
    {
        pmp::Point vPosition = mesh.position(v);
        barycenter += glm::vec3(vPosition[0], vPosition[1], vPosition[2]);
    }
    barycenter /= mesh.n_vertices();

    return barycenter;
}
