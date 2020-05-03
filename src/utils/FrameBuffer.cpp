#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &id_);
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

void FrameBuffer::AttachTexture(TextureSampler texture)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, texture.AttachmentType, GL_TEXTURE_2D, texture.Id, 0);
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