#include "Window.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Fw {
    Window::Window(int width, int height, const std::string& title) : mWindow(nullptr) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!mWindow) {
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            return;
        }
    }

    Window::~Window() {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
        }
        glfwTerminate();
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(mWindow);
    }

    void Window::SwapBuffers() const {
        glfwSwapBuffers(mWindow);
    }

    void Window::PollEvents() const {
        glfwPollEvents();
    }

    void Window::SetShouldClose(bool value) {
        glfwSetWindowShouldClose(mWindow, value);
    }

    void Window::SetTitle(float renderFPS, float videoFPS, int curFrame, int totalFrames) const {
        std::ostringstream oss;
        oss << "Frame Generation | Render FPS: " << std::fixed << std::setprecision(1) << renderFPS
            << " | Video FPS: " << videoFPS << " | Frame: " << curFrame << " / " << totalFrames;
        glfwSetWindowTitle(mWindow, oss.str().c_str());
    }
}
