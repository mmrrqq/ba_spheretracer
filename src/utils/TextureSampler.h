#pragma once

#include "GLUtils.h"

// mostly from https://www.khronos.org/opengl/wiki/Common_Mistakes texture class
class TextureSampler
{
public:
    TextureSampler();
    TextureSampler(unsigned int width,
                   unsigned int height,
                   unsigned int internalFormat,
                   unsigned int format,
                   unsigned int type,
                   unsigned int wrap = GL_CLAMP_TO_EDGE,
                   unsigned int filter = GL_LINEAR,
                   void *data = nullptr);
    TextureSampler(unsigned int width,
                   unsigned int height,
                   unsigned int depth,
                   unsigned int internalFormat,
                   unsigned int format,
                   unsigned int type,
                   void *data = nullptr);
    ~TextureSampler() { release(); };

    TextureSampler(const TextureSampler &) = delete;
    TextureSampler &operator=(const TextureSampler &) = delete;

    TextureSampler(TextureSampler &&other) : id_(other.id_)
    {
        other.id_ = 0;
        other.width_ = 0;
        other.height_ = 0;
        other.depth_ = 0;
    }

    TextureSampler &operator=(TextureSampler &&other)
    {
        //ALWAYS check for self-assignment.
        if (this != &other)
        {
            release();
            //id_ is now 0.
            std::swap(id_, other.id_);
            std::swap(width_, other.width_);
            std::swap(height_, other.height_);
            std::swap(depth_, other.depth_);
        }
    }

    void Bind();
    void Bind(unsigned int slot);
    void BindImage(unsigned int slot, unsigned int mode, unsigned int format);
    void Unbind();

    inline unsigned int GetID() { return id_; }

private:
    void release() { glDeleteTextures(1, &id_); };
    unsigned int id_, height_, width_, depth_;
    std::string uniform_name;
};