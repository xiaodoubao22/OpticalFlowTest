#ifndef VIDEO_RENDER_H
#define VIDEO_RENDER_H

#include "Shader.h"
#include <vector>
#include "Texture.h"
#include "Mesh.h"

namespace Glb {
    class VideoShader : public Shader {
    public:
        VideoShader() = default;
        virtual ~VideoShader() = default;
        void Init();
    };

    class VideoTexture : public Texture {
    public:
        VideoTexture() = default;
        virtual ~VideoTexture() = default;

        void Init(int width, int height);
        void UpdateData(GLenum format, GLenum type, const void* data);
    };

    class VideoRender {
    public:
        VideoRender() = delete;
        VideoRender(int w, int h);
        ~VideoRender();

        void UpdateData(const void* data);
        void Draw();
        void Swap();
        GLuint GetFrameTexId(uint32_t index);

        uint32_t CUR_INDEX = 0U;
        uint32_t PRE_INDEX = 1U;

    private:
        const uint32_t TEXTURE_COUNT = 2U;

        Glb::QuadMesh mQuadMesh;
        std::vector<Glb::VideoTexture> mVideoTexture;
        VideoShader mVideoShader;

        int mWindowWidth = 0;
        int mWindowHeight = 0;
    };
}

#endif // !VIDEO_RENDER_H
