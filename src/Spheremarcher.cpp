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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void Spheremarcher::resize(int width, int height)
{
}

void Spheremarcher::draw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("debug"); // Create a window called "Hello, world!" and append into it.

        ImGui::SliderFloat3("sphere position", &scene_.Spheres[0].position[0], -10.0f, 10.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::Render();
    // TODO: create VertexArray class
    unsigned int vao, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // TODO: create IndexBuffer class
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

    marchingShader_.Bind();
    marchingShader_.SetUniform("UScene", scene_);
    // this enables to draw the screen filling triangle in the vertex shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
    marchingShader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}