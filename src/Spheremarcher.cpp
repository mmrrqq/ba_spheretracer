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

/**
 * @brief Initialize the Application Window.
 * Sets up the VertexArray, the IndexBuffer, Imgui and the scene and loads the shader.
 */
void Spheremarcher::initialize()
{
    initializeImgui();
    initializeScene();

    // TODO: create VertexArray class
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // TODO: create IndexBuffer class
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    activeScene_->SetData();

    // shader_.Load("res/shaders/spheretracing.vertex", "res/shaders/spheretracing.fragment");
    // shader_.Bind();
    // shader_.SetBuffer(3, sizeof(Sphere) * scene_.Spheres.size(), scene_.Spheres.data());
    // shader_.SetBuffer(4, sizeof(Torus) * scene_.Tori.size(), scene_.Tori.data());
    // shader_.SetBuffer(5, sizeof(AreaLight) * scene_.AreaLights.size(), scene_.AreaLights.data());
    // shader_.SetBuffer(6, sizeof(Material) * scene_.Materials.size(), scene_.Materials.data());
    // shader_.SetUniform("UMarchingSteps", 100);
    // shader_.SetUniform("UDrawShadows", drawShadows_);
    // shader_.Unbind();

    generateSdField("res/meshes/amo.off");
}

/**
 * @brief creates the imgui context 
 */
void Spheremarcher::initializeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

/**
 * @brief Sets up some data for the test scene.
 * Fills the scene_ member with the test data primitives.
 */
void Spheremarcher::initializeScene()
{
    Material m1, m2, m3, m4;
    m1.ambientColor = glm::vec4(0.1, 0.1, 0.1, 1);
    m1.diffuseColor = glm::vec4(0.7, 0.7, 0.7, 1);
    m1.specularColor = glm::vec4(0.7, 0.7, 0.7, 1);
    m1.shininess = 10.0f;

    m2.ambientColor = glm::vec4(0.1, 0.0, 0.1, 1);
    m2.diffuseColor = glm::vec4(0.6, 0.0, 0.6, 1);
    m2.specularColor = glm::vec4(0.7, 0.0, 0.7, 1);
    m2.shininess = 10.0f;

    m3.ambientColor = glm::vec4(0.1, 0.1, 0.0, 1);
    m3.diffuseColor = glm::vec4(0.9, 0.9, 0.0, 1);
    m3.specularColor = glm::vec4(0.6, 0.6, 0.0, 1);
    m3.shininess = 10.0f;

    m4.ambientColor = glm::vec4(0.01, 0.1, 0.0, 1);
    m4.diffuseColor = glm::vec4(0.05, 0.2, 0.0, 1);
    m4.specularColor = glm::vec4(0.05, 0.2, 0.0, 1);
    m4.shininess = 10.0f;

    scene_.Materials.push_back(m1);
    scene_.Materials.push_back(m2);
    scene_.Materials.push_back(m3);
    scene_.Materials.push_back(m4);

    Sphere sphere1;
    sphere1.position = glm::vec4(0.5, 0.9, 1.2, 1);
    sphere1.radius = 0.4f;
    sphere1.materialId = 1;
    Sphere sphere2;
    sphere2.position = glm::vec4(0.5, 0.5, 0.5, 1);
    sphere2.radius = 0.6f;
    sphere2.materialId = 0;
    Sphere sphere3;
    sphere3.position = glm::vec4(1.7, 0.0, 1.5, 1);
    sphere3.radius = 0.6f;
    sphere3.materialId = 3;

    Torus torus1;
    torus1.position = glm::vec4(0.5, 0.5, 0.5, 1);
    torus1.materialId = 2;
    torus1.radius = 1;
    torus1.tubeRadius = 0.2;

    Torus torus2;
    torus2.position = glm::vec4(0.0, 4.0, 0.0, 1);
    torus2.materialId = 0;
    torus2.radius = 1;
    torus2.tubeRadius = 0.2;

    // scene_.Spheres.push_back(sphere1);
    // scene_.Spheres.push_back(sphere2);
    // scene_.Spheres.push_back(sphere3);

    // scene_.Tori.push_back(torus1);
    // scene_.Tori.push_back(torus2);

    AreaLight whiteLight;
    whiteLight.position = glm::vec4(4, 6, 5, 1);
    whiteLight.size = 0.01;
    whiteLight.color = glm::vec4(0.9, 0.9, 0.9, 1);

    AreaLight blueLight;
    blueLight.position = glm::vec4(-1, 5, 0, 1);
    blueLight.size = 0.01;
    blueLight.color = glm::vec4(0.2, 0.3, 0.8, 1);

    AreaLight redLight;
    redLight.position = glm::vec4(4, 5, -2, 1);
    redLight.size = 0.1;
    redLight.color = glm::vec4(0.6, 0.2, 0.1, 1);

    AreaLight yellowLight;
    redLight.position = glm::vec4(0, 5, 0, 1);
    redLight.size = 0.01;
    redLight.color = glm::vec4(0.5, 0.5, 0.0, 1);

    scene_.AreaLights.push_back(whiteLight);
    // scene_.AreaLights.push_back(redLight);
    // scene_.AreaLights.push_back(blueLight);
    // scene_.AreaLights.push_back(yellowLight);
}

/**
 * @brief Loads a mesh and generates an SD-Field using the cs.
 * Is able to load any mesh that can be processed by the pmp-library.
 * The cs dispatch call can cause the computer to block and might time out for
 * big resolutions.
 * @param meshPath Path from working directory to the mesh file.
 */
void Spheremarcher::generateSdField(std::string meshPath)
{
    pmp::SurfaceMesh mesh;
    mesh.read(meshPath);
    mesh.triangulate();

    SDFGenerator sdfGenerator(mesh, sdfBoxSize_);
    sdfGenerator_ = std::move(sdfGenerator);

    sdfGenerator_.Dispatch();
    SDField field(glm::vec3(sdfGenerator_.BoxSize), sdfGenerator_.GetOutputTexture());

    field.Scale(sdfScaling_);

    shader_.Bind();
    shader_.SetUniform("USDField", &field, 2);
    shader_.Unbind();
}

/**
 * @brief Window resize callback
 * NOT USED
 * @param width Window width after resize
 * @param height Window height after resize
 */
void Spheremarcher::resize(int width, int height)
{
    // glViewport(0, 0, width, height);
    // SetHeight(height);
    // SetWidth(width);

    // TODO: only allow resize of multipass compatible resoltions
    // TODO: set resoltutions of framebuffers
}

/**
 * @brief Mouse click callback
 * checks if the mouse action is handled by imgui and otherwise
 * sets the mouseDown_ flag member. 
 * @param button 
 * @param action 
 * @param mods 
 */
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

/**
 * @brief Keyboard action callback
 * used for movement with WASD
 * @param key 
 * @param scancode 
 * @param action 
 * @param mods 
 */
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

/**
 * @brief Mouse movement callback. used to rotate the camera when the
 * mouseDown_ flag is set. 
 * @param xpos New window coordinate of the cursor on the x axis
 * @param ypos New window coordinate of the cursor on the y axis
 */
void Spheremarcher::motion(double xpos, double ypos)
{
    if (mouseDown_)
        camera_.Rotate(xpos, ypos, GetWidth(), GetHeight());
}

/**
 * @brief Draw the imgui debug window.
 * Manipulates scene data and updates the shader storage if anything changed.
 * Called every frame.
 */
void Spheremarcher::drawImgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        shader_.Bind();
        ImGui::Begin("debug");
        static int scene;
        const char *sceneNames[] = {"csg", "title", "test"};
        if (ImGui::ListBox("scene_select", &scene, sceneNames, IM_ARRAYSIZE(sceneNames), 3))
            shader_.Load(
                "res/shaders/spheretracing.vertex",
                ("res/shaders/" + (std::string)sceneNames[scene] + ".fragment").c_str());

        ImGui::SliderFloat("fov", &fovy_, 80.0, 120.0);
        ImGui::SliderFloat("normal epsilon", &normalEpsilon_, 0.0001f, 0.01f);
        ImGui::SliderFloat("draw distance", &drawDistance_, 5.0f, 50.0f);
        // ImGui::SliderFloat3("pos", &scene_.Spheres[0].position[0], -5, 5);
        if (ImGui::Checkbox("smoothing", &smooth_))
            shader_.SetUniform("USmooth", smooth_);

        const char *eBoxSizeNames[] = {"32", "64", "128", "256"};
        static int currentSize = 1;
        if (ImGui::ListBox("sdField Box Size", &currentSize, eBoxSizeNames, IM_ARRAYSIZE(eBoxSizeNames), 4))
            sdfBoxSize_ = static_cast<SDFGenerator::EBoxSize>(std::stoi(eBoxSizeNames[currentSize]));

        if (ImGui::Button("regenerate SDF", ImVec2(100, 20)))
        {
            generateSdField("res/meshes/amo.off");
        }

        if (ImGui::SliderFloat("field scaling", &sdfScaling_, 0.001, 0.2))
        {
            shader_.SetUniform("USDField.dimensions", glm::vec3(sdfScaling_ * sdfBoxSize_));
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
                    s.position = glm::vec4(std::rand() / (RAND_MAX / 8.0), std::rand() / (RAND_MAX / 2.0), std::rand() / (RAND_MAX / 8.0), 1);
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

            shader_.SetBuffer(3, sizeof(Sphere) * scene_.Spheres.size(), scene_.Spheres.data());
        }

        if (ImGui::Checkbox("draw shadows", &drawShadows_))
            shader_.SetUniform("UDrawShadows", drawShadows_);

        ImGui::Checkbox("use cone tracing", &coneTracing_);

        ImGui::Text("frame time avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("eye position: (%.3f, %.3f, %.3f)", camera_.GetEye()[0], camera_.GetEye()[1], camera_.GetEye()[2]);
        ImGui::Text("look at position: (%.3f, %.3f, %.3f)", camera_.GetLookAt()[0], camera_.GetLookAt()[1], camera_.GetLookAt()[2]);
        ImGui::End();
    }
    ImGui::Render();
}

/**
 * @brief Draw call that is called every frame.
 * Sets neccessary shader data and either performs a single draw call or
 * multipass conetracing, depending on the class member coneTracing_.
 */
void Spheremarcher::draw()
{
    drawImgui();

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

    shader_.Bind();
    // shader_.SetUniform("USDField", &sdField_, 0U);
    shader_.SetUniform("UMaxDrawDistance", drawDistance_);
    shader_.SetUniform("UNormalEpsilon", normalEpsilon_);
    shader_.SetUniform("UFovY", fovy_);
    shader_.SetUniform("UInvView", glm::inverse(camera_.GetView()));
    shader_.SetUniform("UEyePosition", camera_.GetEye());

    if (coneTracing_)
    {
        firstPassBuffer_.Bind(true);
        shader_.SetUniform("UUseDepthTexture", false);
        shader_.SetUniform("UMarchingSteps", 200);
        shader_.SetUniform("UImageDim", glm::vec2(firstPassBuffer_.GetWidth(), firstPassBuffer_.GetHeight()));
        shader_.SetUniform("UWriteColor", false);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        secondPassBuffer_.Bind(true);
        shader_.SetUniform("UMarchingSteps", 100);
        shader_.SetUniform("UUseDepthTexture", true);
        shader_.SetUniform("UImageDim", glm::vec2(secondPassBuffer_.GetWidth(), secondPassBuffer_.GetHeight()));
        shader_.SetUniform("UDepthTexture", firstPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        thirdPassBuffer_.Bind(true);
        shader_.SetUniform("UMarchingSteps", 100);
        shader_.SetUniform("UImageDim", glm::vec2(thirdPassBuffer_.GetWidth(), thirdPassBuffer_.GetHeight()));
        shader_.SetUniform("UDepthTexture", secondPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        fourthPassBuffer_.Bind(true);
        shader_.SetUniform("UMarchingSteps", 100);
        shader_.SetUniform("UImageDim", glm::vec2(fourthPassBuffer_.GetWidth(), fourthPassBuffer_.GetHeight()));
        shader_.SetUniform("UDepthTexture", thirdPassBuffer_.GetDepthTexture(), 0U);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // DRAW TO SCREEN
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shader_.SetUniform("UWriteColor", true);
        shader_.SetUniform("UMarchingSteps", 100);
        shader_.SetUniform("UDepthTexture", fourthPassBuffer_.GetDepthTexture(), 0U);
    }
    else
    {
        shader_.SetUniform("UUseDepthTexture", false);
        shader_.SetUniform("UWriteColor", true);
        shader_.SetUniform("UMarchingSteps", 200);
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 1920, 1080);
    shader_.SetUniform("UImageDim", glm::vec2(1920, 1080));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    shader_.Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
