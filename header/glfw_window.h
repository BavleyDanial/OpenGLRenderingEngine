#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glfw_input.h>

#include <string>
#include <cstdint>

namespace OGLR {

    struct WindowSpecs {
        std::string title = "OGLR Window";
        uint32_t width = 1280, height = 720;
        bool vsync = true;
        bool fullscreen = false;
    };

    class Window {
    public:
        Window(const WindowSpecs& specs);
        ~Window();

        void OnUpdate() const;
        bool ShouldClose() const { return glfwWindowShouldClose(mGLFWwindow); }
        void Close() const;

        uint32_t GetWidth() const { return mSpecs.width; }
        uint32_t GetHeight() const { return mSpecs.height; }
        bool IsVSync() const { return mSpecs.vsync; }
        bool IsFullScreen() const { return mSpecs.fullscreen; }

        void SetWidth(int width);
        void SetHeight(int height);
        void Resize(int width, int height);

        void Focus() const { Input::SetCurrentWindow(mGLFWwindow); }
    protected:
        GLFWwindow* mGLFWwindow;
        WindowSpecs mSpecs;
    };

}
