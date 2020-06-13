#include "SDFGenerator.h"

SDFGenerator::SDFGenerator()
{
}

SDFGenerator::SDFGenerator(pmp::SurfaceMesh &mesh, float boxSize, float scaleFactor)
    : texInput_(
          (int)mesh.n_faces() * 3,
          1,
          GL_RGBA32F,
          GL_RGBA,
          GL_FLOAT,
          GL_CLAMP_TO_EDGE,
          GL_NEAREST),
      texOutput_((int)boxSize * scaleFactor,
                 (int)boxSize * scaleFactor,
                 (int)boxSize * scaleFactor,
                 GL_RGBA32F,
                 GL_RGBA,
                 GL_FLOAT,
                 GL_CLAMP_TO_EDGE,
                 GL_NEAREST),
      data_(boxSize * scaleFactor * 3 * 4)
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

    texInput_.SetData(&textureData);

    computeShader_.Unbind();
}

// TODO: revisit and implement kd-tree sdf generation
// void SDFGenerator::KDTree(pmp::SurfaceMesh &mesh)
// {
//     pmp::Point bbDim = mesh.bounds().max() - mesh.bounds().min();
//     glm::vec3 dimensions = glm::vec3(bbDim[0], bbDim[1], bbDim[2]);

//     float bbMaximum = std::ceil(glm::compMax(dimensions));

//     OutX = bbMaximum + 0.5, OutY = bbMaximum + 0.5, OutZ = bbMaximum + 0.5;

//     float scaleFactor = 15;

//     OutX *= scaleFactor;
//     OutY *= scaleFactor;
//     OutZ *= scaleFactor;

//     data_ = std::vector<float>(OutZ * OutY * OutX * 4);

//     glm::vec3 barycenter(0.0f);
//     for (auto v : mesh.vertices())
//     {
//         pmp::Point vPosition = mesh.position(v);
//         barycenter += glm::vec3(vPosition[0], vPosition[1], vPosition[2]);
//     }
//     barycenter /= mesh.n_vertices();
//     glm::vec3 toCenter = glm::vec3(bbMaximum / 2.0f) - barycenter;

//     pmp::TriangleKdTree tree(mesh);
//     pmp::TriangleKdTree::Node *currentNode = tree.root;

//     std::stack<pmp::TriangleKdTree::Node *> s;
//     std::stack<unsigned int> parents;

//     std::vector<Node> treeNodes;
//     Triangle triangles[mesh.n_faces()];
//     for (auto f : mesh.faces())
//     {
//         auto face_vertices = mesh.vertices(f);
//         pmp::Point p0 = mesh.position(*face_vertices),
//                    p1 = mesh.position(*(++face_vertices)),
//                    p2 = mesh.position(*(++face_vertices));

//         Triangle t;
//         t.p0 = glm::vec3(p0[0], p0[1], p0[2]);
//         t.p1 = glm::vec3(p1[0], p1[1], p1[2]);
//         t.p2 = glm::vec3(p2[0], p2[1], p2[2]);

//         triangles[f.idx()] = t;
//     }

//     int i = 0;
//     // from: https://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/
//     // while (currentNode != NULL || s.empty() == false)
//     // {
//     //     while (currentNode != NULL)
//     //     {
//     //         s.push(currentNode);
//     //         currentNode = currentNode->left_child;
//     //     }

//     //     currentNode = s.top();
//     //     s.pop();

//     //     // do something with currentNode here
//     //     Node node;
//     //     node.rightNode = currentNode->right_child == nullptr ? -1 : i + 1;
//     //     if (parents.size() < 1)
//     //         node.leftNode = -1;
//     //     else
//     //     {
//     //         node.leftNode = parents.top();
//     //         parents.pop();
//     //     }

//     //     for (auto triangle : *currentNode->faces)
//     //     {
//     //         node.triangles.push_back(triangle.f.idx());
//     //     }

//     //     treeNodes.push_back(node);

//     //     parents.push(i++);

//     //     currentNode = currentNode->right_child;
//     // }

//     computeShader_.Load("res/shaders/sdfGenerator.compute");
//     computeShader_.Bind();

//     TextureSampler texOutput(
//         (int)OutX,
//         (int)OutY,
//         (int)OutZ,
//         GL_RGBA32F,
//         GL_RGBA,
//         GL_FLOAT,
//         GL_CLAMP_TO_EDGE,
//         GL_LINEAR);

//     // texInput_ = std::move(texInput);
//     texOutput_ = std::move(texOutput);

//     texInput_.BindImage(0, GL_READ_ONLY, GL_RGBA32F);
//     texOutput_.BindImage(1, GL_READ_WRITE, GL_RGBA32F);

//     computeShader_.Unbind();
// }

void SDFGenerator::Generate(std::vector<float> *data)
{
    computeShader_.Bind();

    texInput_.BindImage(0, GL_READ_ONLY, GL_RGBA32F);
    texOutput_.BindImage(1, GL_READ_WRITE, GL_RGBA32F);

    glDispatchCompute(OutX, OutY, OutZ);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    data_ = std::vector<float>(OutX * OutY * OutZ * 4);

    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, data_.data());
    computeShader_.Unbind();

    data->reserve(data_.size() / 4.0);

    int i;
    for (i = 0; i < data_.size(); i += 4)
    {
        data->push_back(data_.data()[i]);
    }
}