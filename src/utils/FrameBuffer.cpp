#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &id_);
}

FrameBuffer::FrameBuffer(int width, int height)
    : colorTexture_(width, height, GL_COLOR_ATTACHMENT0),
      depthTexture_(width, height, GL_DEPTH_ATTACHMENT)
{
    glGenFramebuffers(1, &id_);
    Bind();
    colorTexture_.Bind();
    AttachTexture(colorTexture_);
    depthTexture_.Bind();
    AttachTexture(depthTexture_);

    CheckStatus();
    Unbind();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id_);
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AttachTexture(TextureSampler &texture)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, texture.GetAttachmentType(), GL_TEXTURE_2D, texture.GetID(), 0);
}

unsigned int FrameBuffer::CheckStatus()
{
    if (unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "error::framebuffer::not_complete" << std::endl;
        return status;
    }

    Unbind();
    return 0;
}