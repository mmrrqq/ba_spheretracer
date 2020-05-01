#include "Spheremarcher.h"

Spheremarcher::Spheremarcher(int width, int height)
    : Window("Spheremarcher", width, height), marchingShader_(), mouseDown_(false), moving_(false)
{
}

Spheremarcher::~Spheremarcher()
{
}

void Spheremarcher::initialize()
{
    std::vector<Material> materials;
    materials.push_back(
        Material(
            glm::vec3(0.2, 0.0, 0.0),
            glm::vec3(0.7, 0.7, 0.0),
            glm::vec3(0.7, 0.7, 0.7),
            10.0f));

    materials.push_back(
        Material(
            glm::vec3(0.0, 0.0, 0.2),
            glm::vec3(0.7, 0.0, 0.7),
            glm::vec3(0.7, 0.7, 0.7),
            10.0f));

    Sphere testSphere;
    testSphere.position = glm::vec3(0.0, 0.0, 0.0);
    testSphere.radius = 1.0f;
    testSphere.materialId = 1;

    // Torus testTorus;
    // testTorus.position = glm::vec3(-2.0, 0.1, -5.0);
    // testTorus.radius = 2.0f;
    // testTorus.tubeRadius = 0.5f;
    // testTorus.materialId = 0;

    scene_.AddSphere(testSphere);
    // scene_.AddTorus(testTorus);
    marchingShader_.Load("res/shaders/marching.vertex", "res/shaders/marching.fragment");

    marchingShader_.Bind();
    marchingShader_.SetUniform("UImageDim", glm::vec2(GetWidth(), GetHeight()));
    marchingShader_.SetUniform("UNormalEpsilon", 0.003f);
    marchingShader_.SetUniform("ULightDirection", glm::normalize(glm::vec3(-1.0f, -1.0f, 0.5f)));
    marchingShader_.SetUniform("UScene", scene_);
    marchingShader_.SetUniform("UMaterials", materials);
    marchingShader_.SetUniform("UInvView", camera_.GetView());
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
    glViewport(0, 0, width, height);
    SetHeight(height);
    SetWidth(width);

    marchingShader_.Bind();
    marchingShader_.SetUniform("UImageDim", glm::vec2(width, height));
    marchingShader_.Unbind();
}

void Spheremarcher::mouse(int button, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        // mouse is above imgui window..
    }
    else
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            if (action == GLFW_PRESS)
                mouseDown_ = true;
            else if (action == GLFW_RELEASE)
            {
                mouseDown_ = false;
                camera_.Reset();
            }
        }
    }
}

void Spheremarcher::keyboard(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        moving_ = true;
    if (action == GLFW_RELEASE)
        moving_ = false;

    if (moving_)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            camera_.Move(glm::vec2(0.05f, 0));
            break;

        case GLFW_KEY_S:
            camera_.Move(glm::vec2(-0.05f, 0));
            break;

        default:
            break;
        }
    }
}

void Spheremarcher::motion(double xpos, double ypos)
{

    if (mouseDown_)
        camera_.Rotate(xpos, ypos, GetWidth(), GetHeight());
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
    // marchingShader_.SetUniform("UScene", scene_);
    marchingShader_.SetUniform("UInvView", glm::inverse(camera_.GetView()));
    marchingShader_.SetUniform("UEyePosition", camera_.GetEye());
    // this enables to draw the screen filling triangle in the vertex shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
    marchingShader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}