//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#include "Window.h"
#include <iostream>
//=============================================================================

//-----------------------------------------------------------------------------

Window::Window(const char *title, int width, int height) : width_(width), height_(height)
{
    // initialize glfw window
    if (!glfwInit())
    {
        std::cerr << "Cannot initialize GLFW!\n";
        exit(EXIT_FAILURE);
    }

    // request core profile and OpenGL version 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // TODO: am I allowed to request 4.3? error callback got introduced there..

    // try to create window
    window_ = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window_)
    {
        glfwTerminate();
        std::cerr << "Window creation failed!\n";
        exit(EXIT_FAILURE);
    }

    // make this window the current one
    glfwMakeContextCurrent(window_);

    // enable vsync
    glfwSwapInterval(1);

    // this sets a reference to the window which can be retrieved in every callback
    // https://www.glfw.org/docs/latest/group__window.html#ga3d2fc6026e690ab31a13f78bc9fd3651
    glfwSetWindowUserPointer(window_, (void *)this);
    // register glfw callbacks
    glfwSetKeyCallback(window_, keyboard__);
    glfwSetMouseButtonCallback(window_, mouse__);
    glfwSetFramebufferSizeCallback(window_, resize__);
    glfwSetCursorPosCallback(window_, motion__);
    glfwSetScrollCallback(window_, scroll__);

    // now that we have a GL context, initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    GLenum error = glGetError();

    //define blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST); // this has to be enabled to write to gl_FragDepth as it seems

    // debug: print GL and GLSL version
    std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // set debug error https://learnopengl.com/In-Practice/Debugging
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&error__, nullptr);
    }
}

//-----------------------------------------------------------------------------

Window::~Window()
{
}

//-----------------------------------------------------------------------------

int Window::run()
{
    // initialize OpenGL
    initialize();

    // query framebuffer width and height
    // call resize to initialize viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    resize(width, height);

    // now run the event loop
    while (!glfwWindowShouldClose(window_))
    {
        // call timer function
        timer();

        // draw scene
        draw();

        // swap buffers
        glfwSwapBuffers(window_);

        // handle events
        glfwPollEvents();
    }

    glfwDestroyWindow(window_);
    glfwTerminate();

    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------

void Window::error__(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message, const void *userParam)
{
    // TODO: refine error message handling
    std::cout << message << std::endl;
    std::cout << source << std::endl;
}

void Window::keyboard__(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Window *windowRef = (Window *)glfwGetWindowUserPointer(window);
    windowRef->keyboard(key, scancode, action, mods);
}

void Window::mouse__(GLFWwindow *window, int button, int action, int mods)
{
    Window *windowRef = (Window *)glfwGetWindowUserPointer(window);
    windowRef->mouse(button, action, mods);
}

void Window::motion__(GLFWwindow *window, double xpos, double ypos)
{
    Window *windowRef = (Window *)glfwGetWindowUserPointer(window);
    windowRef->motion(xpos, ypos);
}

void Window::scroll__(GLFWwindow *window, double xoffset, double yoffset)
{
    Window *windowRef = (Window *)glfwGetWindowUserPointer(window);
    windowRef->scroll(xoffset, yoffset);
}

void Window::resize__(GLFWwindow *window, int width, int height)
{
    Window *windowRef = (Window *)glfwGetWindowUserPointer(window);
    windowRef->resize(width, height);
}

//=============================================================================
