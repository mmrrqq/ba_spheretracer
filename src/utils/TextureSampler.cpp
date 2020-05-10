#include "TextureSampler.h"

#include "GLUtils.h"

TextureSampler::TextureSampler() {}

TextureSampler::TextureSampler(unsigned int width, unsigned int height, unsigned int attachmentType)
{
    width_ = width;
    height_ = height;
    attachmentType_ = attachmentType;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    switch (attachmentType)
    {
    case GL_COLOR_ATTACHMENT0:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_INT, 0);
        break;
    case GL_DEPTH_ATTACHMENT:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        break;

    default:
        throw "error::TextureSampler::constructor::attachmentType::invalid";
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureSampler::Bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
}

void TextureSampler::Bind()
{
    glBindTexture(GL_TEXTURE_2D, id_);
}

void TextureSampler::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}