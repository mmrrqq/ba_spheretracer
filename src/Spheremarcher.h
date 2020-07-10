#pragma once

#include "FrameBuffer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pmp/SurfaceMesh.h"
#include "utils/Camera.h"
#include "utils/PrimitiveScene.h"
#include "utils/SDFGenerator.h"
#include "utils/SDField.h"
#include "utils/Shader.h"
#include "utils/Window.h"

class Spheremarcher : public Window
{
public:
    Spheremarcher(int width, int height);
    ~Spheremarcher();

private:
    virtual void initialize();
    virtual void resize(int width, int height);
    virtual void draw();

    virtual void keyboard(int key, int scancode, int action, int mods);
    virtual void mouse(int button, int action, int mods);
    virtual void motion(double xpos, double ypos);

    void initializeImgui();
    void drawImgui();
    void setShaderData();

    FrameBuffer firstPassBuffer_, secondPassBuffer_, thirdPassBuffer_,
        fourthPassBuffer_;
    Shader shader_;
    Camera camera_;
    PrimitiveScene scene_;
    SDFGenerator::EBoxSize sdfBoxSize_;
    bool mouseDown_, moving_, smooth_, drawShadows_, coneTracing_;
    unsigned int vao_, ibo_;
    float fovy_, normalEpsilon_, drawDistance_, sdfScaling_;
};
