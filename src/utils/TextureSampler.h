#pragma once

#include <iostream>

#include "GLUtils.h"

// mostly from https://www.khronos.org/opengl/wiki/Common_Mistakes texture
/**
 * @brief Class to represent an OpenGL texture sampler.
 * Both 2D and 3D textures can be represented.
 */
class TextureSampler
{
public:
    TextureSampler();
    TextureSampler(unsigned int width, unsigned int height,
                   unsigned int internalFormat, unsigned int format,
                   unsigned int type, unsigned int wrap, unsigned int filter);
    TextureSampler(unsigned int width, unsigned int height, unsigned int depth,
                   unsigned int internalFormat, unsigned int format,
                   unsigned int type, unsigned int wrap, unsigned int filter);
    ~TextureSampler() { release(); };

    TextureSampler(const TextureSampler &) = delete;
    TextureSampler &operator=(const TextureSampler &) = delete;

    TextureSampler(TextureSampler &&other) : id_(other.id_)
    {
        other.id_ = 0;
        other.width_ = 0;
        other.height_ = 0;
        other.depth_ = 0;
        other.internalFormat_ = 0;
        other.type_ = 0;
        other.format_ = 0;
    }

    TextureSampler &operator=(TextureSampler &&other)
    {
        // ALWAYS check for self-assignment.
        if (this != &other)
        {
            release();
            // id_ is now 0.
            std::swap(id_, other.id_);
            std::swap(width_, other.width_);
            std::swap(height_, other.height_);
            std::swap(depth_, other.depth_);
            std::swap(internalFormat_, other.internalFormat_);
            std::swap(format_, other.format_);
            std::swap(type_, other.type_);
        }
    }

    void Bind(unsigned int slot);
    void BindImage(unsigned int slot, unsigned int mode, unsigned int format);
    void Unbind();

    void SetData(void *data);

    inline unsigned int ID() { return id_; }

private:
    void release()
    {
        glDeleteTextures(1, &id_);
        id_ = height_ = width_ = depth_, internalFormat_, type_, format_ = 0;
    };
    unsigned int id_, height_, width_, depth_, internalFormat_, type_,
        format_ = 0;
};
