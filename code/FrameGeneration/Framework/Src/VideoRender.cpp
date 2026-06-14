#include "VideoRender.h"
#include <iostream>

namespace Glb {
    void VideoShader::Init()
    {
        std::string vertPath = "../code/FrameGeneration/Framework/Shaders/VideoFrame.vert";
        std::string fragPath = "../code/FrameGeneration/Framework/Shaders/VideoFrame.frag";
        if (Glb::Shader::BuildFromFile(vertPath, fragPath) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
        Glb::Shader::Use();
        Glb::Shader::SetInt("videoTexture", 0);
        Glb::Shader::UnUse();
    }

    void VideoTexture::Init(int width, int height)
    {
        Texture::Create();
        Texture::Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        Texture::Allocate(width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        Texture::UnBind();
    }

    void VideoTexture::UpdateData(GLenum format, GLenum type, const void* data) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Texture::GetWidth(), Texture::GetHeight(), format, type, data);
    }

    VideoRender::VideoRender(int w, int h)
    {
        mVideoTexture.resize(TEXTURE_COUNT);
        for (uint32_t i = 0; i < mVideoTexture.size(); i++) {
            mVideoTexture[i].Init(w, h);
        }
        mQuadMesh.Create();
        mVideoShader.Init();
        mWindowWidth = w;
        mWindowHeight = h;
    }

    VideoRender::~VideoRender()
    {

    }

    void VideoRender::UpdateData(const void* data)
    {
        mVideoTexture[CUR_INDEX].Bind();
        mVideoTexture[CUR_INDEX].UpdateData(GL_RGB, GL_UNSIGNED_BYTE, data);
        mVideoTexture[CUR_INDEX].UnBind();
    }

    void VideoRender::Draw()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, mWindowWidth, mWindowHeight);

        mVideoShader.Use();
        glActiveTexture(GL_TEXTURE0);
        mVideoTexture[CUR_INDEX].Bind();
        mQuadMesh.Draw();
    }

    void VideoRender::Swap()
    {
        std::swap(CUR_INDEX, PRE_INDEX);
    }

    GLuint VideoRender::GetFrameTexId(uint32_t index)
    {
        return mVideoTexture[index].GetId();
    }

} // namespace Glb