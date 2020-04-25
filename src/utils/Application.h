#pragma once

#include "Window.h"

class Application
{
public:
    Application(char *name, Window *window);
    ~Application();
    void run();

private:
    char *name;
    Window *window_;
};