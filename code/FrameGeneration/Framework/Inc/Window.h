#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Fw {
    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        bool ShouldClose() const;
        void SwapBuffers() const;
        void PollEvents() const;
        void SetShouldClose(bool value);
        void SetTitle(float renderFPS, float videoFPS, int curFrame, int totalFrames) const;

    private:
        GLFWwindow* mWindow;
    };
}

#endif
