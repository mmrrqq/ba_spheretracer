#include "Spheremarcher.h"

#include "FrameBuffer.h"
#include "TextureSampler.h"

Spheremarcher::Spheremarcher(int width, int height)
    : Window("Spheremarcher", width, height),
      mouseDown_(false),
      moving_(false)
{
}

Spheremarcher::~Spheremarcher()
{
}

void Spheremarcher::initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    /////// SCENE TEST SETUP
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
    scene_.AddSphere(testSphere);
    /////// END SCENE TEST SETUP

    // TODO: create VertexArray class
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // TODO: create IndexBuffer class
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // framebuffer setup
    colorTexture_ = TextureSampler(GetWidth(), GetHeight(), GL_COLOR_ATTACHMENT0);
    depthTexture_ = TextureSampler(GetWidth(), GetHeight(), GL_DEPTH_ATTACHMENT);

    offScreenBuffer_.Bind();
    colorTexture_.Bind();
    offScreenBuffer_.AttachTexture(colorTexture_);
    depthTexture_.Bind();
    offScreenBuffer_.AttachTexture(depthTexture_);
    offScreenBuffer_.CheckStatus();
    offScreenBuffer_.Unbind();

    offScreenShader_.Load("res/shaders/marching.vertex", "res/shaders/offScreenShader.fragment");
    screenShader_.Load("res/shaders/marching.vertex", "res/shaders/screenShader.fragment");

    offScreenShader_.Bind();
    offScreenShader_.SetUniform("UImageDim", glm::vec2(GetWidth(), GetHeight()));
    offScreenShader_.SetUniform("UNormalEpsilon", 0.003f);
    offScreenShader_.SetUniform("ULightDirection", glm::normalize(glm::vec3(-1.0f, -1.0f, 0.5f)));
    offScreenShader_.SetUniform("UScene", scene_);
    offScreenShader_.SetUniform("UMaterials", materials);
    offScreenShader_.SetUniform("UInvView", camera_.GetView());
    offScreenShader_.Unbind();
}

void Spheremarcher::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    SetHeight(height);
    SetWidth(width);

    offScreenShader_.Bind();
    offScreenShader_.SetUniform("UImageDim", glm::vec2(width, height));
    offScreenShader_.Unbind();
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

    // FIRST RENDER PASS TO FRAMEBUFFER
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    offScreenBuffer_.Bind();
    offScreenShader_.Bind();

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    offScreenShader_.SetUniform("UInvView", glm::inverse(camera_.GetView()));
    offScreenShader_.SetUniform("UEyePosition", camera_.GetEye());
    // this enables to draw the screen filling triangle in the vertex shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
    offScreenBuffer_.Unbind();
    offScreenShader_.Unbind();

    // DRAW TO SCREEN
    screenShader_.Bind();

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    screenShader_.SetUniform("UColorTexture", colorTexture_, 0U);
    screenShader_.SetUniform("UDepthTexture", depthTexture_, 1U);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    screenShader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}