#pragma once

#include "GLUtils.h"
#include "TextureSampler.h"
#include <iostream>

class FrameBuffer
{
public:
    FrameBuffer();
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    void AttachTexture(TextureSampler &texture);
    unsigned int CheckStatus();

    TextureSampler &GetColorTexture() { return colorTexture_; };
    TextureSampler &GetDepthTexture() { return depthTexture_; };

private:
    unsigned int id_;

    TextureSampler colorTexture_;
    TextureSampler depthTexture_;
};