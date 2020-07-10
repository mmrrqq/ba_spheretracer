#pragma once

#include <iostream>

#include "GLUtils.h"
#include "TextureSampler.h"

/**
 * @brief Framebuffer class.
 * Holds the framebuffers color and depth textures.
 */
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

    TextureSampler &ColorTexture() { return colorTexture_; };
    TextureSampler &DepthTexture() { return depthTexture_; };

private:
    unsigned int checkStatus();

    unsigned int id_, width_, height_;

    TextureSampler colorTexture_;
    TextureSampler depthTexture_;
};