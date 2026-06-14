#ifndef FSR3_OPTICAL_FLOW_H
#define FSR3_OPTICAL_FLOW_H

#include "ComputeShader.h"
#include "Shader.h"
#include <vector>
#include "Texture.h"
#include "Mesh.h"

namespace Alg {
    struct Extent2D
    {
        uint32_t w;
        uint32_t h;
    };

    class SearchingShader : public Glb::ComputeShader {
    public:
        SearchingShader() : ComputeShader("SearchingShader") {}
        virtual ~SearchingShader() = default;
        void Init();
    };

    class FilteringShader : public Glb::ComputeShader {
    public:
        FilteringShader() : ComputeShader("FilteringShader") {}
        virtual ~FilteringShader() = default;
        void Init();
    };

    class UpscalingShader : public Glb::ComputeShader {
    public:
        UpscalingShader() : ComputeShader("UpscalingShader") {}
        virtual ~UpscalingShader() = default;
        void Init();
    };

    class PrepareLumaShader : public Glb::Shader {
    public:
        PrepareLumaShader() = default;
        virtual ~PrepareLumaShader() = default;
        void Init();
    };

    class DownSampleLumaShader : public Glb::Shader {
    public:
        DownSampleLumaShader() = default;
        virtual ~DownSampleLumaShader() = default;
        void Init();
    };

    class InterpolationShader : public Glb::Shader {
    public:
        InterpolationShader() = default;
        virtual ~InterpolationShader() = default;
        void Init();
    };

    class FSR3OpticalFlow {
    public:
        FSR3OpticalFlow(int inputW, int inputH);
        ~FSR3OpticalFlow();

        void Draw(GLuint curColor, GLuint preColor);
        
    private:
        void UpdateResolution();
        void GenTextures();
        void GenMipTextures(std::vector<Extent2D>& sizes, std::vector<Glb::Texture>& textures,
            GLenum internalFormat, GLenum format, GLenum type);
        void GenBlitFbs();
        void Blit(Glb::Texture& srcTexture, Glb::Texture& dstTexture, GLenum filter);
        void BlitEqualSizeTexture(GLuint srcTexture, GLuint dstTexture, int w, int h, GLenum filter);
        void PrepareLuma(GLuint oriColor);
        void DownSampleLuma();

        PrepareLumaShader mPrepareLumaShader;
        DownSampleLumaShader mDownSampleLumaShader;
        SearchingShader mSearchingShader;
        FilteringShader mFilteringShader;
        UpscalingShader mUpscalingShader;
        InterpolationShader mInterpolationShader;

        Glb::QuadMesh mQuadMesh;

        const uint32_t LAYER_COUNT = 5U;
        const uint32_t BLOCK_SIZE = 8U;
        const uint32_t GROUP_OF_SIZE = 2U;
        const uint32_t FILTER_DISPATCH_X_SIZE = 16U;
        const uint32_t FILTER_DISPATCH_Y_SIZE = 4U;

        int mInputW = 0;
        int mInputH = 0;

        std::vector<Extent2D> mLumaSizes;
        std::vector<Extent2D> mOFSizes;
        std::vector<Glb::Texture> mCurLumaMips;
        std::vector<Glb::Texture> mPreLumaMips;
        std::vector<Glb::Texture> mOFMipsSearched;
        std::vector<Glb::Texture> mOFMipsFiltered;
        Glb::Texture mPrevColor;

        GLuint mBlitReadFb = 0;
        GLuint mBlitWriteFb = 0;
    };
}

#endif // !FSR3_OPTICAL_FLOW_H
