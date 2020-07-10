#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() { glGenFramebuffers(1, &id_); }

/**
 * @brief Construct a new Frame Buffer:: Frame Buffer object
 * Creates a new FrameBuffer Object and initializes the color and depth
 * textures with the specified size.
 * @param width
 * @param height
 */
FrameBuffer::FrameBuffer(int width, int height)
    : width_(width),
      height_(height),
      colorTexture_(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT,
                    GL_CLAMP_TO_EDGE, GL_LINEAR),
      depthTexture_(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT,
                    GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST)
{
    glGenFramebuffers(1, &id_);
    Bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           colorTexture_.ID(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depthTexture_.ID(), 0);

    checkStatus();
}

FrameBuffer::~FrameBuffer() { glDeleteFramebuffers(1, &id_); }

void FrameBuffer::Bind(bool clearDepth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    if (clearDepth) glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width_, height_);
}

void FrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

unsigned int FrameBuffer::checkStatus()
{
    if (unsigned int status =
            glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "error::framebuffer::not_complete" << std::endl;
        return status;
    }

    Unbind();
    return 0;
}