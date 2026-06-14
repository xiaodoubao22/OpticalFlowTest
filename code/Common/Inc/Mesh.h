#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <vector>

namespace Glb {
    class Mesh {
    public:
        Mesh();
        virtual ~Mesh();

        virtual void Create();
        virtual void Destroy();

        void SetVertices(const float* vertices, size_t size);
        void SetIndices(const unsigned int* indices, size_t count);

        void AddAttribute(GLuint index, GLint size, GLsizei stride, size_t offset);

        virtual void Draw(GLenum mode = GL_TRIANGLES) const;

    private:
        GLuint mVAO = 0;
        GLuint mVBO = 0;
        GLuint mEBO = 0;
        size_t mIndexCount = 0;
    };

    class QuadMesh : public Mesh {
    public:
        QuadMesh() = default;
        ~QuadMesh() = default;

        void Create() override;

    private:
        float QUAD_VERTICES[20] = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };
        unsigned int QUAD_INDICES[6] = { 0, 1, 3, 1, 2, 3 };
    };
}

#endif
