#include "Spheremarcher.h"

#include "FrameBuffer.h"
#include "TextureSampler.h"

Spheremarcher::Spheremarcher(int width, int height)
    : Window("Spheremarcher", width, height),
      mouseDown_(false),
      moving_(false),
      firstPassBuffer_(GetWidth() / 64, GetHeight() / 64),
      secondPassBuffer_(GetWidth() / 32, GetHeight() / 32),
      thirdPassBuffer_(GetWidth() / 8, GetHeight() / 8),
      fourthPassBuffer_(GetWidth() / 2, GetHeight() / 2),
      fovy_(90.0f)
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
    pmp::SurfaceMesh mesh;
    mesh.read("res/meshes/tree.obj");
    mesh.triangulate();
    sdfGenerator_ = std::move(SDFGenerator(mesh));

    sdfGenerator_.Generate(&sdField_);

    std::vector<Material> materials;
    materials.push_back(
        Material(
            glm::vec3(0.1, 0.1, 0.1),
            glm::vec3(0.7, 0.7, 0.7),
            glm::vec3(0.7, 0.7, 0.7),
            10.0f));

    materials.push_back(
        Material(
            glm::vec3(0.1, 0.0, 0.1),
            glm::vec3(0.6, 0.0, 0.6),
            glm::vec3(0.7, 0.0, 0.7),
            10.0f));

    materials.push_back(
        Material(
            glm::vec3(0.1, 0.3, 0.0),
            glm::vec3(0.2, 0.8, 0.0),
            glm::vec3(0.1, 0.7, 0.0),
            10.0f));

    Sphere sphere1;
    sphere1.position = glm::vec3(1.5, 0.5, 0.0);
    sphere1.radius = 0.2f;
    sphere1.materialId = 1;
    sphere1.morph = true;
    Sphere sphere2;
    sphere2.position = glm::vec3(0.0, 0.5, 0.0);
    sphere2.radius = 0.1f;
    sphere2.materialId = 0;
    sphere2.morph = true;
    Sphere sphere3;
    sphere3.position = glm::vec3(1.5, 0.5, -1.7);
    sphere3.radius = 0.3f;
    sphere3.materialId = 2;
    sphere3.morph = true;

    scene_.AddSphere(sphere1);
    scene_.AddSphere(sphere2);
    scene_.AddSphere(sphere3);

    SceneLights lights;
    PointLight yellowLight;
    yellowLight.position = glm::vec3(4, 4, 0);
    yellowLight.size = 0.05;
    yellowLight.color = glm::vec3(0.7, 0.7, 0.2);
    PointLight whiteLight;
    whiteLight.position = glm::vec3(-4, 6, 0);
    whiteLight.size = 0.03;
    whiteLight.color = glm::vec3(0.8, 0.8, 0.8);

    PointLight redLight;
    redLight.position = glm::vec3(0, 5, -4);
    redLight.size = 0.005;
    redLight.color = glm::vec3(0.7, 0.5, 0.1);

    lights.pointLights.push_back(whiteLight);
    // lights.pointLights.push_back(yellowLight);
    lights.pointLights.push_back(redLight);
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

    screenShader_.Load("res/shaders/marching.vertex", "res/shaders/screenShader.fragment");

    screenShader_.Bind();
    screenShader_.SetUniform("USDField", &sdField_, 1);
    screenShader_.SetUniform("UNormalEpsilon", 0.007f);
    screenShader_.SetUniform("UScene", scene_);
    screenShader_.SetUniform("ULights", lights);
    screenShader_.SetUniform("UMaterials", materials);
    screenShader_.SetUniform("UMarchingSteps", 100);
    screenShader_.SetUniform("UMaxDrawDistance", 27.0f);
    screenShader_.Unbind();
}

void Spheremarcher::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    SetHeight(height);
    SetWidth(width);

    // offScreenShader_.Bind();
    // offScreenShader_.SetUniform("UImageDim", glm::vec2(width, height));
    // offScreenShader_.Unbind();

    // TODO: only allow resize of multipass compatible resoltions
    // TODO: set resoltutions of framebuffers
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
        ImGui::Begin("debug");
        ImGui::SliderFloat("fov", &fovy_, 80.0, 120.0);
        ImGui::SliderFloat3("pos", &scene_.Spheres[0].position[0], -5, 5);

        ImGui::Text("frame time avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::Render();

    // glEnable(GL_DEPTH_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    screenShader_.Bind();

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    firstPassBuffer_.Bind(true);
    // screenShader_.SetUniform("UScene", scene_);
    // screenShader_.SetUniform("USDField", &sdField_, 1);
    screenShader_.SetUniform("UFovY", fovy_);
    screenShader_.SetUniform("UMarchingSteps", 100);
    screenShader_.SetUniform("UInvView", glm::inverse(camera_.GetView()));
    screenShader_.SetUniform("UEyePosition", camera_.GetEye());
    screenShader_.SetUniform("UImageDim", glm::vec2(firstPassBuffer_.GetWidth(), firstPassBuffer_.GetHeight()));
    screenShader_.SetUniform("UUseDepthTexture", false);
    screenShader_.SetUniform("UWriteColor", false);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    secondPassBuffer_.Bind(true);
    screenShader_.SetUniform("UMarchingSteps", 100);
    screenShader_.SetUniform("UUseDepthTexture", true);
    screenShader_.SetUniform("UImageDim", glm::vec2(secondPassBuffer_.GetWidth(), secondPassBuffer_.GetHeight()));
    screenShader_.SetUniform("UDepthTexture", firstPassBuffer_.GetDepthTexture(), 0U);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    thirdPassBuffer_.Bind(true);
    screenShader_.SetUniform("UMarchingSteps", 60);
    screenShader_.SetUniform("UImageDim", glm::vec2(thirdPassBuffer_.GetWidth(), thirdPassBuffer_.GetHeight()));
    screenShader_.SetUniform("UDepthTexture", secondPassBuffer_.GetDepthTexture(), 0U);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    fourthPassBuffer_.Bind(true);
    screenShader_.SetUniform("UMarchingSteps", 60);
    screenShader_.SetUniform("UImageDim", glm::vec2(fourthPassBuffer_.GetWidth(), fourthPassBuffer_.GetHeight()));
    screenShader_.SetUniform("UDepthTexture", thirdPassBuffer_.GetDepthTexture(), 0U);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // DRAW TO SCREEN
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 1920, 1080);
    screenShader_.SetUniform("UWriteColor", true);
    screenShader_.SetUniform("UMarchingSteps", 60);
    screenShader_.SetUniform("UImageDim", glm::vec2(1920, 1080));
    screenShader_.SetUniform("UDepthTexture", fourthPassBuffer_.GetDepthTexture(), 0U);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    screenShader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}