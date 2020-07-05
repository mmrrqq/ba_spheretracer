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
      fovy_(90.0f),
      normalEpsilon_(0.005f),
      drawDistance_(27.0f),
      smooth_(false),
      sdfBoxSize_(SDFGenerator::B_128),
      sdfScaling_(0.01),
      drawShadows_(true),
      coneTracing_(true)
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
            glm::vec3(0.1, 0.1, 0.0),
            glm::vec3(0.9, 0.9, 0.0),
            glm::vec3(0.6, 0.6, 0.0),
            10.0f));

    materials.push_back(
        Material(
            glm::vec3(0.01, 0.1, 0.0),
            glm::vec3(0.05, 0.2, 0.0),
            glm::vec3(0.05, 0.2, 0.0),
            10.0f));

    Sphere sphere1;
    sphere1.position = glm::vec3(0.5, 0.9, 1.2);
    sphere1.radius = 0.4f;
    sphere1.materialId = 1;
    Sphere sphere2;
    sphere2.position = glm::vec3(0.5, 0.5, 0.5);
    sphere2.radius = 0.6f;
    sphere2.materialId = 0;
    Sphere sphere3;
    sphere3.position = glm::vec3(1.7, 0.0, 1.5);
    sphere3.radius = 0.6f;
    sphere3.materialId = 3;

    Torus torus1;
    torus1.position = glm::vec3(0.5, 0.5, 0.5);
    torus1.materialId = 2;
    torus1.radius = 1;
    torus1.tubeRadius = 0.2;

    Torus torus2;
    torus2.position = glm::vec3(0.0, 4.0, 0.0);
    torus2.materialId = 0;
    torus2.radius = 1;
    torus2.tubeRadius = 0.2;

    // scene_.AddSphere(sphere1);
    // scene_.AddSphere(sphere2);
    // scene_.AddSphere(sphere3);

    // scene_.AddTorus(torus1);
    // scene_.AddTorus(torus2);

    SceneLights lights;
    PointLight whiteLight;
    whiteLight.position = glm::vec3(4, 6, 5);
    whiteLight.size = 0.01;
    whiteLight.color = glm::vec3(0.9, 0.9, 0.9);

    PointLight blueLight;
    blueLight.position = glm::vec3(-1, 5, 0);
    blueLight.size = 0.01;
    blueLight.color = glm::vec3(0.2, 0.3, 0.8);

    PointLight redLight;
    redLight.position = glm::vec3(4, 5, -2);
    redLight.size = 0.1;
    redLight.color = glm::vec3(0.6, 0.2, 0.1);

    PointLight yellowLight;
    redLight.position = glm::vec3(0, 5, 0);
    redLight.size = 0.01;
    redLight.color = glm::vec3(0.5, 0.5, 0.0);

    lights.pointLights.push_back(whiteLight);
    // lights.pointLights.push_back(redLight);
    // lights.pointLights.push_back(blueLight);
    // lights.pointLights.push_back(yellowLight);
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
    screenShader_.SetBuffer(3, sizeof(Sphere) * scene_.Spheres.size(), scene_.Spheres.data());
    screenShader_.SetBuffer(4, sizeof(Torus) * scene_.Tori.size(), scene_.Tori.data());
    screenShader_.SetUniform("ULights", lights);
    screenShader_.SetUniform("UMaterials", materials);
    screenShader_.SetUniform("UMarchingSteps", 100);
    screenShader_.SetUniform("UDrawShadows", drawShadows_);
    screenShader_.Unbind();

    generateSdField();
}

void Spheremarcher::generateSdField()
{
    pmp::SurfaceMesh mesh;
    mesh.read("res/meshes/amo.off");
    mesh.triangulate();

    SDFGenerator sdfGenerator(mesh, sdfBoxSize_);
    sdfGenerator_ = std::move(sdfGenerator);

    sdfGenerator_.Dispatch();
    SDField field(glm::vec3(sdfGenerator_.BoxSize), sdfGenerator_.GetOutputTexture());

    field.Scale(sdfScaling_);

    screenShader_.Bind();
    screenShader_.SetUniform("USDField", &field, 2);
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

void Spheremarcher::drawImguiWindow()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        screenShader_.Bind();
        ImGui::Begin("debug");
        ImGui::SliderFloat("fov", &fovy_, 80.0, 120.0);
        ImGui::SliderFloat("normal epsilon", &normalEpsilon_, 0.0001f, 0.01f);
        ImGui::SliderFloat("draw distance", &drawDistance_, 5.0f, 50.0f);
        // ImGui::SliderFloat3("pos", &scene_.Spheres[0].position[0], -5, 5);
        ImGui::Checkbox("smoothing", &smooth_);

        const char *eBoxSizeNames[] = {"32", "64", "128", "256"};
        static int currentSize = 1;
        if (ImGui::ListBox("sdField Box Size", &currentSize, eBoxSizeNames, IM_ARRAYSIZE(eBoxSizeNames), 4))
            sdfBoxSize_ = static_cast<SDFGenerator::EBoxSize>(std::stoi(eBoxSizeNames[currentSize]));

        if (ImGui::Button("regenerate SDF", ImVec2(100, 20)))
        {
            generateSdField();
        }

        if (ImGui::SliderFloat("field scaling", &sdfScaling_, 0.001, 0.2))
        {
            screenShader_.SetUniform("USDField.dimensions", glm::vec3(sdfScaling_ * sdfBoxSize_));
        }

        float lightSize = 0.01;
        if (ImGui::SliderFloat("light size", &lightSize, 0.001, 0.5))
        {
            screenShader_.SetUniform("ULights.pointLights[0].size", lightSize);
        }

        int spherecount = scene_.Spheres.size();
        if (ImGui::SliderInt("sphere count", &spherecount, 1, 100))
        {
            int countDiff = spherecount - scene_.Spheres.size();
            if (countDiff > 0)
            {
                for (int i = 0; i < countDiff; i++)
                {
                    Sphere s;
                    s.materialId = std::rand() % 3;
                    s.position = glm::vec3(std::rand() / (RAND_MAX / 8.0), std::rand() / (RAND_MAX / 2.0), std::rand() / (RAND_MAX / 8.0));
                    s.radius = std::rand() / (float)RAND_MAX;
                    scene_.Spheres.push_back(s);
                }
            }
            else
            {
                scene_.Spheres.erase(
                    scene_.Spheres.end() + countDiff,
                    scene_.Spheres.end());
            }

            screenShader_.SetBuffer(3, sizeof(Sphere) * scene_.Spheres.size(), scene_.Spheres.data());
        }

        if (ImGui::Checkbox("draw shadows", &drawShadows_))
            screenShader_.SetUniform("UDrawShadows", drawShadows_);

        ImGui::Checkbox("use cone tracing", &coneTracing_);

        ImGui::Text("frame time avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("eye position: (%.3f, %.3f, %.3f)", camera_.GetEye()[0], camera_.GetEye()[1], camera_.GetEye()[2]);
        ImGui::Text("look at position: (%.3f, %.3f, %.3f)", camera_.GetLookAt()[0], camera_.GetLookAt()[1], camera_.GetLookAt()[2]);
        ImGui::End();
    }
    ImGui::Render();
}

void Spheremarcher::draw()
{
    drawImguiWindow();

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    screenShader_.Bind();
    // screenShader_.SetUniform("USDField", &sdField_, 0U);
    screenShader_.SetUniform("UMaxDrawDistance", drawDistance_);
    screenShader_.SetUniform("UNormalEpsilon", normalEpsilon_);
    screenShader_.SetUniform("UFovY", fovy_);
    screenShader_.SetUniform("UInvView", glm::inverse(camera_.GetView()));
    screenShader_.SetUniform("UEyePosition", camera_.GetEye());
    screenShader_.SetUniform("USmooth", smooth_);

    if (coneTracing_)
    {
        firstPassBuffer_.Bind(true);
        screenShader_.SetUniform("UUseDepthTexture", false);
        screenShader_.SetUniform("UMarchingSteps", 200);
        screenShader_.SetUniform("UImageDim", glm::vec2(firstPassBuffer_.GetWidth(), firstPassBuffer_.GetHeight()));
        screenShader_.SetUniform("UWriteColor", false);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        secondPassBuffer_.Bind(true);
        screenShader_.SetUniform("UMarchingSteps", 100);
        screenShader_.SetUniform("UUseDepthTexture", true);
        screenShader_.SetUniform("UImageDim", glm::vec2(secondPassBuffer_.GetWidth(), secondPassBuffer_.GetHeight()));
        screenShader_.SetUniform("UDepthTexture", firstPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        thirdPassBuffer_.Bind(true);
        screenShader_.SetUniform("UMarchingSteps", 100);
        screenShader_.SetUniform("UImageDim", glm::vec2(thirdPassBuffer_.GetWidth(), thirdPassBuffer_.GetHeight()));
        screenShader_.SetUniform("UDepthTexture", secondPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        fourthPassBuffer_.Bind(true);
        screenShader_.SetUniform("UMarchingSteps", 100);
        screenShader_.SetUniform("UImageDim", glm::vec2(fourthPassBuffer_.GetWidth(), fourthPassBuffer_.GetHeight()));
        screenShader_.SetUniform("UDepthTexture", thirdPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // DRAW TO SCREEN
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 1920, 1080);
        screenShader_.SetUniform("UWriteColor", true);
        screenShader_.SetUniform("UMarchingSteps", 100);
        screenShader_.SetUniform("UImageDim", glm::vec2(1920, 1080));
        screenShader_.SetUniform("UDepthTexture", fourthPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    else
    {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 1920, 1080);
        screenShader_.SetUniform("UUseDepthTexture", false);
        screenShader_.SetUniform("UWriteColor", true);
        screenShader_.SetUniform("UMarchingSteps", 200);
        screenShader_.SetUniform("UImageDim", glm::vec2(1920, 1080));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    screenShader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
