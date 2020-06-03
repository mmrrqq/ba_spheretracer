#include "TextureSampler.h"

#include "GLUtils.h"

TextureSampler::TextureSampler() {}

TextureSampler::TextureSampler(
    unsigned int width,
    unsigned int height,
    unsigned int internalFormat,
    unsigned int format,
    unsigned int type,
    unsigned int wrap,
    unsigned int filter,
    void *data)
{
    width_ = width;
    height_ = height;
    depth_ = 0;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, format, type, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureSampler::TextureSampler(
    unsigned int width,
    unsigned int height,
    unsigned int depth,
    unsigned int internalFormat,
    unsigned int format,
    unsigned int type,
    void *data)
{
    width_ = width;
    height_ = height;
    depth_ = depth;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_3D, id_);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width_, height_, depth_, 0, format, type, data);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void TextureSampler::Bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    depth_ == 0 ? glBindTexture(GL_TEXTURE_2D, id_) : glBindTexture(GL_TEXTURE_3D, id_);
}

void TextureSampler::Bind()
{
    depth_ == 0 ? glBindTexture(GL_TEXTURE_2D, id_) : glBindTexture(GL_TEXTURE_3D, id_);
}

void TextureSampler::BindImage(unsigned int slot, unsigned int mode, unsigned int format)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    if (depth_ == 0)
    {
        glBindTexture(GL_TEXTURE_2D, id_);
        glBindImageTexture(slot, id_, 0, GL_FALSE, 0, mode, format);
    }
    else
    {
        glBindTexture(GL_TEXTURE_3D, id_);
        glBindImageTexture(slot, id_, 0, GL_TRUE, 0, mode, format);
    }
}

void TextureSampler::Unbind()
{
    depth_ == 0 ? glBindTexture(GL_TEXTURE_2D, 0) : glBindTexture(GL_TEXTURE_3D, 0);
}