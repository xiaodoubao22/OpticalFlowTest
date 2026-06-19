#include <iostream>
#include <chrono>
#include <thread>
#include "Framework/Inc/Window.h"
#include "Framework/Inc/VideoLoader.h"
#include "Framework/Inc/FPSCounter.h"
#include "VideoRender.h"
#include "FSR3OpticalFlow.h"

const bool CONTROL_FPS = false;
const std::string VIDEO_PATH = "../resources/Videos/Seq-004/Seq-004.mp4";


const std::chrono::microseconds TARGET_FRAME_DURATION(9000);
auto frameStartTime = std::chrono::high_resolution_clock::now();

void ControlFPS()
{
    if (!CONTROL_FPS) {
        return;
    }
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);

    if (elapsed < TARGET_FRAME_DURATION)
    {
        std::chrono::microseconds sleepNeeded = TARGET_FRAME_DURATION - elapsed;
        std::this_thread::sleep_for(sleepNeeded);
    }
}

int main()
{
    Fw::VideoLoader videoLoader;
    if (!videoLoader.Open(VIDEO_PATH)) {
        std::cerr << "Failed to open video: " << VIDEO_PATH << std::endl;
        return -1;
    }
    int videoWidth = videoLoader.GetWidth();
    int videoHeight = videoLoader.GetHeight();
    std::cout << "w=" << videoWidth << " h=" << videoHeight << std::endl;

    Fw::Window window(videoWidth, videoHeight, VIDEO_PATH.c_str());

    Glb::VideoRender videoRender(videoWidth, videoHeight);

    Alg::FSR3OpticalFlow fsr3of(videoWidth, videoHeight);

    Fw::VideoFrame frame;
    Fw::FPSCounter renderFPSCounter;
    Fw::FPSCounter videoFPSCounter;
    while (!window.ShouldClose()) {

        frameStartTime = std::chrono::high_resolution_clock::now();

        videoLoader.ReadFrame(frame);
        if (!videoLoader.ReadFrame(frame)) {
            videoLoader.Close();
            videoLoader.Open(VIDEO_PATH);
            std::cout << "Video looped" << std::endl;
            continue;
        }
        videoRender.Swap();

        videoRender.UpdateData(frame.data[0]);
        videoFPSCounter.Tick();

        window.PollEvents();
        
        fsr3of.Draw(videoRender.GetFrameTexId(videoRender.CUR_INDEX), videoRender.GetFrameTexId(videoRender.PRE_INDEX));
        renderFPSCounter.Tick();
        window.SwapBuffers();
        ControlFPS();
        
        frameStartTime = std::chrono::high_resolution_clock::now();

        videoRender.Draw();
        renderFPSCounter.Tick();
        window.SetTitle(renderFPSCounter.GetFPS(), videoFPSCounter.GetFPS(), videoLoader.GetCurrentFrame(), videoLoader.GetTotalFrames());
        window.SwapBuffers();
        ControlFPS();
    }

    videoLoader.Close();

    return 0;
}
