//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#pragma once
//=============================================================================

#include <stdlib.h>
#include "GLUtils.h"

//== CLASS DEFINITION =========================================================

/// \addtogroup glfw_window glfw_window
/// @{

/// Simple GLFW window
class Window
{
public:
    Window(const char *_title = "", int _width = 0, int _height = 0);
    virtual ~Window();

    /// main window loop
    int run();

    inline void SetWidth(int width) { width_ = width; }
    inline void SetHeight(int height) { height_ = height; }

    inline int GetWidth() { return width_; }
    inline int GetHeight() { return height_; }

protected:
    /// must overload
    virtual void initialize() = 0;
    virtual void resize(int width, int height) = 0;
    virtual void draw() = 0;

    // may overload
    virtual void keyboard(int key, int scancode, int action, int mods) {}
    virtual void mouse(int button, int action, int mods) {}
    virtual void motion(double xpos, double ypos) {}
    virtual void scroll(double xoffset, double yoffset) {}
    virtual void timer() {}

    GLFWwindow *window_;

private:
    static void APIENTRY error__(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar *message, const void *userParam);
    static void keyboard__(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse__(GLFWwindow *window, int button, int action, int mods);
    static void resize__(GLFWwindow *window, int width, int height);
    static void motion__(GLFWwindow *window, double xpos, double ypos);
    static void scroll__(GLFWwindow *window, double xoffset, double yoffset);

    int width_, height_;
};

/// @}

//=============================================================================
//=============================================================================
