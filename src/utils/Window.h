//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//   MODIFIED by mrothgaenger
//
//=============================================================================
#pragma once
//=============================================================================

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//== CLASS DEFINITION =========================================================

/// \addtogroup glfw_window glfw_window
/// @{

/// Simple GLFW window
class Window
{

public: //------------------------------------------------------ public methods
    /// constructor
    Window(const char *_title = "", int _width = 0, int _height = 0);

    /// destructor
    virtual ~Window();

    /// main window loop
    int run();

    inline int GetWidth() { return width_; }
    inline int GetHeight() { return height_; }
    inline void SetWidth(int width) { width_ = width; }
    inline void SetHeight(int height) { height_ = height; }

private:
    int width_, height_;
    //----------------------------- static wrapper functions for callbacks
    static void APIENTRY error__(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar *message, const void *userParam);
    static void keyboard__(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse__(GLFWwindow *window, int button, int action, int mods);
    static void resize__(GLFWwindow *window, int width, int height);
    static void motion__(GLFWwindow *window, double xpos, double ypos);
    static void scroll__(GLFWwindow *window, double xoffset, double yoffset);

protected: //----------------------------------- callbacks as member functions
    /// must overload: initialize all OpenGL states
    virtual void initialize() = 0;

    /// must overload: handle window resizing
    virtual void resize(int width, int height) = 0;

    /// must overload: render the scene
    virtual void draw() = 0;

    /// may overload: handle keyboard events
    virtual void keyboard(int key, int scancode, int action, int mods) {}

    /// may overload: handle keyboard events
    virtual void mouse(int button, int action, int mods) {}

    //! this function handles mouse motion (passive/active position)
    virtual void motion(double xpos, double ypos) {}

    //! this function handles mouse scroll events
    virtual void scroll(double xoffset, double yoffset) {}

    /// may overload: handle idle timer
    virtual void timer() {}

protected: //----------------------------------------------------- protected data
    /// GLFW window pointer
    GLFWwindow *window_;
};

/// @}

//=============================================================================
//=============================================================================
