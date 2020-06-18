#include "TextureSampler.h"

TextureSampler::TextureSampler() : id_(0), height_(0), width_(0), depth_(0), internalFormat_(0), type_(0), format_(0) {}

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

    if (width_ < 1 || height_ < 1)
    {
        std::cout << "warning::TextureSampler::constructor::width:height:lt:1" << std::endl;
        std::cout << "warning::TextureSampler::no:texture:created" << std::endl;
        return;
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTextureStorage2D(id_, 1, internalFormat_, width_, height_);

    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, wrap);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, filter);
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

    if (width_ < 1 || height_ < 1 || depth_ < 1)
    {
        std::cout << "warning::TextureSampler::constructor::width:height:depth:lt:1" << std::endl;
        std::cout << "warning::TextureSampler::no:texture:created" << std::endl;
        return;
    }

    glGenTextures(1, &id_);
    // glCreateTextures(GL_TEXTURE_3D, 1, &id_);
    glBindTexture(GL_TEXTURE_3D, id_);
    // glTextureStorage3D(id_, 1, internalFormat_, width_, height_, depth_);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width_, height_, depth_, 0, format, type, nullptr);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void TextureSampler::Bind(unsigned int slot)
{
    glBindTextureUnit(slot, id_);
}

void TextureSampler::BindImage(unsigned int slot, unsigned int mode, unsigned int format)
{
    if (depth_ == 0)
    {
        glBindImageTexture(slot, id_, 0, GL_FALSE, 0, mode, format);
    }
    else
    {
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
