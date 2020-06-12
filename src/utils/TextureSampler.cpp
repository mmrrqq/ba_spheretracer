#include "TextureSampler.h"

TextureSampler::TextureSampler() {}

TextureSampler::TextureSampler(
    unsigned int width,
    unsigned int height,
    unsigned int internalFormat,
    unsigned int format,
    unsigned int type,
    unsigned int wrap,
    unsigned int filter)
{
    width_ = width;
    height_ = height;
    depth_ = 0;
    internalFormat_ = internalFormat;
    format_ = format;
    type_ = type;

    // glGenTextures(1, &id_);
    // glBindTexture(GL_TEXTURE_2D, id_);

    glCreateTextures(GL_TEXTURE_2D, 1, &id_);
    glTextureStorage2D(id_, 1, internalFormat_, width, height_);

    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, wrap);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, filter);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, filter);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, format, type, data);
}

TextureSampler::TextureSampler(
    unsigned int width,
    unsigned int height,
    unsigned int depth,
    unsigned int internalFormat,
    unsigned int format,
    unsigned int type,
    unsigned int wrap,
    unsigned int filter)
{
    width_ = width;
    height_ = height;
    depth_ = depth;
    internalFormat_ = internalFormat;
    format_ = format;
    type_ = type;

    // glGenTextures(1, &id_);
    glCreateTextures(GL_TEXTURE_3D, 1, &id_);
    glTextureStorage3D(id_, 1, internalFormat_, width, height_, depth_);
    // glBindTexture(GL_TEXTURE_3D, id_);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, wrap);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_R, wrap);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, filter);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, filter);
    // glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width_, height_, depth_, 0, format, type, data);

    // glBindTexture(GL_TEXTURE_3D, 0);
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

void TextureSampler::SetData(void *data)
{
    if (depth_ == 0)
    {
        glTextureSubImage2D(id_, 0, 0, 0, width_, height_, format_, type_, data);
    }
    else
    {
        glTextureSubImage3D(id_, 0, 0, 0, 0, width_, height_, depth_, format_, type_, data);
    }
}