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

    void Bind(bool clearDepth = false);
    void Unbind();

    int Width() { return width_; };
    int Height() { return height_; };

    void AttachTexture(TextureSampler &texture, unsigned int attachmentType);
    unsigned int CheckStatus();

    TextureSampler &ColorTexture() { return colorTexture_; };
    TextureSampler &DepthTexture() { return depthTexture_; };

private:
    unsigned int id_, width_, height_;

    TextureSampler colorTexture_;
    TextureSampler depthTexture_;
};