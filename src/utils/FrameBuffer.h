#pragma once

#include "GLUtils.h"
#include "TextureSampler.h"
#include <iostream>

class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind();
    void Unbind();

    void AttachTexture(TextureSampler &texture);
    unsigned int CheckStatus();

private:
    unsigned int id_;
};