#include "FSR3OpticalFlow.h"
#include <iostream>

namespace Alg {
    void SearchingShader::Init()
    {
        std::string path = "../code/FrameGeneration/Algorithm/Shaders/Searching.comp";
        if (Glb::ComputeShader::BuildFromFile(path) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
    }

    void FilteringShader::Init()
    {
        std::string path = "../code/FrameGeneration/Algorithm/Shaders/Filtering.comp";
        if (Glb::ComputeShader::BuildFromFile(path) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
    }

    void UpscalingShader::Init()
    {
        std::string path = "../code/FrameGeneration/Algorithm/Shaders/Upscaling.comp";
        if (Glb::ComputeShader::BuildFromFile(path) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
    }

    void PrepareLumaShader::Init()
    {
        std::string vertPath = "../code/FrameGeneration/Algorithm/Shaders/PrepareLuma.vert";
        std::string fragPath = "../code/FrameGeneration/Algorithm/Shaders/PrepareLuma.frag";
        if (Glb::Shader::BuildFromFile(vertPath, fragPath) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
        Glb::Shader::Use();
        Glb::Shader::SetInt("oriTex", 0);
        Glb::Shader::UnUse();
    }

    void DownSampleLumaShader::Init()
    {
        std::string vertPath = "../code/FrameGeneration/Algorithm/Shaders/PrepareLuma.vert";
        std::string fragPath = "../code/FrameGeneration/Algorithm/Shaders/DownSampleLuma.frag";
        if (Glb::Shader::BuildFromFile(vertPath, fragPath) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
    }

    void InterpolationShader::Init()
    {
        std::string vertPath = "../code/FrameGeneration/Algorithm/Shaders/PrepareLuma.vert";
        std::string fragPath = "../code/FrameGeneration/Algorithm/Shaders/Interpolation.frag";
        if (Glb::Shader::BuildFromFile(vertPath, fragPath) != 0) {
            std::cerr << "Failed to build shader" << std::endl;
            return;
        }
        Glb::Shader::Use();
        Glb::Shader::SetInt("oriTex", 0);
        Glb::Shader::UnUse();
    }


    FSR3OpticalFlow::FSR3OpticalFlow(int inputW, int inputH)
    {
        mQuadMesh.Create();
        mPrepareLumaShader.Init();
        mDownSampleLumaShader.Init();
        mSearchingShader.Init();
        mFilteringShader.Init();
        mUpscalingShader.Init();
        mInterpolationShader.Init();
        mInputW = inputW;
        mInputH = inputH;
        UpdateResolution();
        GenTextures();
        GenBlitFbs();
    }

    FSR3OpticalFlow::~FSR3OpticalFlow()
    {

    }


    void FSR3OpticalFlow::Draw(GLuint curColor, GLuint preColor)
    {
        std::swap(mPreLumaMips, mCurLumaMips);
        PrepareLuma(curColor);
        DownSampleLuma();

        glm::ivec2 resolution = glm::ivec2(mLumaSizes.back().w, mLumaSizes.back().h);
        
        
        for (uint32_t i = 0; i < LAYER_COUNT; i++) {
            uint32_t uOpticalFlowPyramidLevel = LAYER_COUNT - i - 1;

            mSearchingShader.Use();
            mSearchingShader.SetIVec2(std::string("iInputLumaResolution"), resolution);
            mSearchingShader.SetUInt(std::string("uOpticalFlowPyramidLevel"), uOpticalFlowPyramidLevel);
            mSearchingShader.SetUInt(std::string("uOpticalFlowPyramidLevelCount"), LAYER_COUNT);
            mSearchingShader.SetUInt(std::string("iFrameIndex"), 100U);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mCurLumaMips[i].GetId());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mPreLumaMips[i].GetId());
            glBindImageTexture(2, mOFMipsSearched[i].GetId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG16I);
            glDispatchCompute((mOFSizes[i].w + GROUP_OF_SIZE - 1) / GROUP_OF_SIZE,
                (mOFSizes[i].h + GROUP_OF_SIZE - 1) / GROUP_OF_SIZE, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            
            mFilteringShader.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mOFMipsSearched[i].GetId());
            glBindImageTexture(1, mOFMipsFiltered[i].GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16I);
            glDispatchCompute((mOFSizes[i].w + FILTER_DISPATCH_X_SIZE - 1) / FILTER_DISPATCH_X_SIZE,
                (mOFSizes[i].h + FILTER_DISPATCH_Y_SIZE - 1) / FILTER_DISPATCH_Y_SIZE, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


            if (i < LAYER_COUNT - 1) {
                mUpscalingShader.Use();
                mUpscalingShader.SetIVec2(std::string("iInputLumaResolution"), resolution);
                mUpscalingShader.SetUInt(std::string("uOpticalFlowPyramidLevel"), uOpticalFlowPyramidLevel);
                mUpscalingShader.SetUInt(std::string("uOpticalFlowPyramidLevelCount"), LAYER_COUNT);
                mUpscalingShader.SetUInt(std::string("iFrameIndex"), 100U);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mCurLumaMips[i].GetId());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mPreLumaMips[i].GetId());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, mOFMipsFiltered[i].GetId());
                glBindImageTexture(3, mOFMipsSearched[i + 1].GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16I);
                glDispatchCompute((mOFSizes[i].w + GROUP_OF_SIZE - 1) / GROUP_OF_SIZE,
                    (mOFSizes[i].h + GROUP_OF_SIZE - 1) / GROUP_OF_SIZE, 1);
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            }
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, mInputW, mInputH);

        glm::ivec2 ofResolution = glm::ivec2(mOFSizes.back().w, mOFSizes.back().h);

        mInterpolationShader.Use();
        mInterpolationShader.SetIVec2(std::string("uInputLumaResolution"), resolution);
        mInterpolationShader.SetIVec2(std::string("uOpticalFlowResolution"), ofResolution);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, preColor);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, curColor);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mOFMipsFiltered.back().GetId());
        mQuadMesh.Draw();
    }

    void FSR3OpticalFlow::UpdateResolution()
    {
        int minW = (mInputW >> (LAYER_COUNT - 1));
        int minH = (mInputH >> (LAYER_COUNT - 1));

        Extent2D tmpExtent = { minW , minH };
        mLumaSizes.resize(LAYER_COUNT);
        mOFSizes.resize(LAYER_COUNT);
        for (uint32_t i = 0; i < LAYER_COUNT; i++) {
            mLumaSizes[i] = tmpExtent;
            mOFSizes[i] = {
                (tmpExtent.w + BLOCK_SIZE - 1) / BLOCK_SIZE, // 向上取整
                (tmpExtent.h + BLOCK_SIZE - 1) / BLOCK_SIZE,
            };
            tmpExtent.w = tmpExtent.w << 1U;
            tmpExtent.h = tmpExtent.h << 1U;
        }
    }

    void FSR3OpticalFlow::GenTextures()
    {
        GenMipTextures(mLumaSizes, mPreLumaMips, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
        GenMipTextures(mLumaSizes, mCurLumaMips, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
        GenMipTextures(mOFSizes, mOFMipsSearched, GL_RG16I, GL_RG_INTEGER, GL_SHORT);
        GenMipTextures(mOFSizes, mOFMipsFiltered, GL_RG16I, GL_RG_INTEGER, GL_SHORT);

        mPrevColor.Create();
        mPrevColor.Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mPrevColor.Allocate(mInputW, mInputH, GL_RGB8, GL_UNSIGNED_BYTE, nullptr);
        mPrevColor.UnBind();
    }

    void FSR3OpticalFlow::GenMipTextures(std::vector<Extent2D>& sizes, std::vector<Glb::Texture>& textures,
        GLenum internalFormat, GLenum format, GLenum type)
    {
        textures.resize(sizes.size());
        for (uint32_t i = 0; i < LAYER_COUNT; i++) {
            textures[i].Create();
            textures[i].Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, sizes[i].w, sizes[i].h, 0, format, type, nullptr);
            textures[i].UnBind();
        }
    }

    void FSR3OpticalFlow::GenBlitFbs()
    {
        glGenFramebuffers(1, &mBlitReadFb);
        glGenFramebuffers(1, &mBlitWriteFb);
    }

    void FSR3OpticalFlow::Blit(Glb::Texture& srcTexture, Glb::Texture& dstTexture, GLenum filter)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mBlitReadFb);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture.GetId(), 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlitWriteFb);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture.GetId(), 0);

        glBlitFramebuffer(
            0, 0, srcTexture.GetWidth(), srcTexture.GetHeight(),
            0, 0, dstTexture.GetWidth(), dstTexture.GetHeight(),
            GL_COLOR_BUFFER_BIT, filter
        );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void FSR3OpticalFlow::BlitEqualSizeTexture(GLuint srcTexture, GLuint dstTexture, int w, int h, GLenum filter)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mBlitReadFb);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlitWriteFb);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);

        glBlitFramebuffer(
            0, 0, w, h,
            0, 0, w, h,
            GL_COLOR_BUFFER_BIT, filter
        );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void FSR3OpticalFlow::PrepareLuma(GLuint oriColor)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlitWriteFb);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mCurLumaMips.back().GetId(), 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, mLumaSizes.back().w, mLumaSizes.back().h);

        mPrepareLumaShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, oriColor);
        mQuadMesh.Draw();
    }

    void FSR3OpticalFlow::DownSampleLuma()
    {
        for (uint32_t i = LAYER_COUNT - 1; i > 0; i--) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlitWriteFb);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mCurLumaMips[i - 1].GetId(), 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, mLumaSizes[i - 1].w, mLumaSizes[i - 1].h);

            mDownSampleLumaShader.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mCurLumaMips[i].GetId());
            mQuadMesh.Draw();
        }
    }

} // namespace Glb