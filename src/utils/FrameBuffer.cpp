#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() { glGenFramebuffers(1, &id_); }

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
    AttachTexture(colorTexture_, GL_COLOR_ATTACHMENT0);
    AttachTexture(depthTexture_, GL_DEPTH_ATTACHMENT);

    CheckStatus();
}

FrameBuffer::~FrameBuffer() { glDeleteFramebuffers(1, &id_); }

void FrameBuffer::Bind(bool clearDepth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    if (clearDepth) glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width_, height_);
}

void FrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::AttachTexture(TextureSampler &texture,
                                unsigned int attachmentType)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D,
                           texture.ID(), 0);
}

unsigned int FrameBuffer::CheckStatus()
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