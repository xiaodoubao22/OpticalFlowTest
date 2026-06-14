#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

namespace Glb {
    class Texture {
    public:
        Texture();
        virtual ~Texture();

        virtual void Create();
        virtual void Destroy();

        void Bind() const;
        void UnBind() const;

        void Allocate(int width, int height, GLenum format, GLenum type, const void* data);
        void Update(int xoffset, int yoffset, int width, int height, GLenum format, GLenum type, const void* data);

        GLuint GetId() const;
        int GetWidth() const;
        int GetHeight() const;

    private:
        GLuint mId = 0;
        int mWidth = 0;
        int mHeight = 0;
    };
}

#endif
