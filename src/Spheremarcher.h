#pragma once

#include "utils/Window.h"
#include "utils/Camera.h"
#include "utils/Shader.h"
#include "utils/PrimitiveScene.h"
#include "utils/SDFGenerator.h"
#include "utils/SDField.h"
#include "FrameBuffer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pmp/SurfaceMesh.h"

class Spheremarcher : public Window
{
public:
    Spheremarcher(int width, int height);
    ~Spheremarcher();

private:
    FrameBuffer firstPassBuffer_, secondPassBuffer_, thirdPassBuffer_, fourthPassBuffer_;
    Shader screenShader_;
    PrimitiveScene scene_;
    Camera camera_;
    bool mouseDown_, moving_, smooth_;
    unsigned int vao_, ibo_;
    float fovy_; //, normalEpsilon_, drawDistance_;
    // SDFGenerator sdfGenerator_;
    std::vector<float> sdfData_;

    /// function that is called on the creation of the widget for the initialisation of OpenGL
    virtual void initialize();

    /// resize function - called when the window size is changed
    virtual void resize(int width, int height);

    // /// draw function - called when the window should be refreshed
    virtual void draw();

    // /// keyboard interaction
    virtual void keyboard(int key, int scancode, int action, int mods);

    // /// mouse interaction
    virtual void mouse(int button, int action, int mods);

    // /// mouse motion
    virtual void motion(double xpos, double ypos);

    // /// mouse wheel
    // virtual void scroll(double xoffset, double yoffset);
};
