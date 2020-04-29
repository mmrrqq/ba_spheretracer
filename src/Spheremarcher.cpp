#include "Spheremarcher.h"

Spheremarcher::Spheremarcher(int width, int height)
    : Window("Spheremarcher", width, height), marchingShader_()
{
}

Spheremarcher::~Spheremarcher()
{
}

void Spheremarcher::initialize()
{
    Sphere testSphere;
    testSphere.position = glm::vec3(1.0, 1.0, -5.0);
    testSphere.radius = 1.0f;
    Torus testTorus;
    testTorus.position = glm::vec3(-2.0, 0.1, -5.0);
    testTorus.radius = 2.0f;
    testTorus.tubeRadius = 0.5f;
    scene_.AddSphere(testSphere);
    scene_.AddTorus(testTorus);
    marchingShader_.Load("res/shaders/marching.vertex", "res/shaders/marching.fragment");

    marchingShader_.Bind();
    marchingShader_.SetUniform("UImageDim", glm::vec2(GetWidth(), GetHeight()));
    marchingShader_.SetUniform("UNormalEpsilon", 0.003f);
    marchingShader_.SetUniform("ULightDirection", glm::normalize(glm::vec3(-1.0f, -1.0f, 0.5f)));
    marchingShader_.SetUniform("UScene", scene_);
    marchingShader_.Unbind();
}

void Spheremarcher::resize(int width, int height)
{
}

void Spheremarcher::draw()
{
    // TODO: create VertexArray class
    unsigned int vao, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // TODO: create IndexBuffer class
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

    marchingShader_.Bind();
    // this enables to draw the screen filling triangle in the vertex shader
    glDrawArrays(GL_TRIANGLES, 0, 3);

    marchingShader_.Unbind();
}