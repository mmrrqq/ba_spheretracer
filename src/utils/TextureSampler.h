#pragma once

#include "GLUtils.h"

// mostly from https://www.khronos.org/opengl/wiki/Common_Mistakes texture class
class TextureSampler
{
public:
    TextureSampler();
    TextureSampler(unsigned int width, unsigned int height, unsigned int attachmentType);
    ~TextureSampler() { release(); };

    TextureSampler(const TextureSampler &) = delete;
    TextureSampler &operator=(const TextureSampler &) = delete;

    TextureSampler(TextureSampler &&other) : id_(other.id_)
    {
        other.id_ = 0; //Use the "null" texture for the old object.
        other.width_ = 0;
        other.height_ = 0;
        other.attachmentType_ = 0;
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
            std::swap(attachmentType_, other.attachmentType_);
        }
    }

    void Bind();
    void Bind(unsigned int slot);
    void Unbind();

    inline unsigned int GetID() { return id_; }
    inline unsigned int GetAttachmentType() { return attachmentType_; }

private:
    void release() { glDeleteTextures(1, &id_); };
    unsigned int id_, height_, width_, attachmentType_;
    std::string uniform_name;
};