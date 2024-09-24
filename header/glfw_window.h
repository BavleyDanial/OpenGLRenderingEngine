#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glfw_input.h>

#include <string>
#include <cstdint>

namespace OGLR {

    struct WindowSpecs {
        std::string title = "OGLR Window";
        uint32_t width = 480, height = 680;
        bool vsync = true;
        bool fullscreen = false;
    };

    class Window {
    public:
        Window(const WindowSpecs& specs);
        ~Window();

        void OnUpdate();
        bool ShouldClose() const { return glfwWindowShouldClose(mGLFWwindow); }
        void Close();

        // TODO: create Setters for these
        uint32_t GetWidth() const { return mSpecs.width; }
        uint32_t GetHeight() const { return mSpecs.width; }
        bool IsVSync() const { return mSpecs.vsync; }
        bool IsFullScreen() const { return mSpecs.fullscreen; }

        void Focus() { Input::SetCurrentWindow(mGLFWwindow); }
    protected:
        GLFWwindow* mGLFWwindow;
        WindowSpecs mSpecs;
    };

}
