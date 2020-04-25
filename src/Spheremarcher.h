#pragma once

#include "utils/Window.h"
#include "utils/Shader.h"

class Spheremarcher : public Window
{
public:
    Spheremarcher(int width, int height);
    ~Spheremarcher();

private:
    Shader marchingShader_;
    /// function that is called on the creation of the widget for the initialisation of OpenGL
    virtual void initialize();

    /// resize function - called when the window size is changed
    virtual void resize(int width, int height);

    // /// draw function - called when the window should be refreshed
    virtual void draw();

    // /// keyboard interaction
    // virtual void keyboard(int key, int scancode, int action, int mods);

    // /// mouse interaction
    // virtual void mouse(int button, int action, int mods);

    // /// mouse motion
    // virtual void motion(double xpos, double ypos);

    // /// mouse wheel
    // virtual void scroll(double xoffset, double yoffset);
};
