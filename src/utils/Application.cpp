#include "Application.h"

Application::Application(char *name, Window *window) : name(name), window_(window)
{
}

Application::~Application()
{
    window_->~Window();
}

void Application::run()
{
    window_->run();
}