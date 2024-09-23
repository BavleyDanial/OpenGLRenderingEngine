#include <glfw_window.h>
#include <cassert>

#include <glad/glad.h>

namespace OGLR {

    Window::Window(const WindowSpecs& specs)
    :mGLFWwindow(nullptr), mSpecs(specs) {

        if (!glfwInit())
            assert("Couldn't initialise glfw");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        mGLFWwindow = glfwCreateWindow(mSpecs.width, mSpecs.height, mSpecs.title.c_str(), nullptr, nullptr);
        if (!mGLFWwindow) {
            glfwTerminate();
            assert("Couldn't initialise window");
        }

        glfwMakeContextCurrent(mGLFWwindow);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            assert("Failed to initialize GLAD");
        }

        glViewport(0, 0, mSpecs.width, mSpecs.height);

        if (mSpecs.vsync)
            glfwSwapInterval(1);

        if (mSpecs.fullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(mGLFWwindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }

    Window::~Window() {
        glfwDestroyWindow(mGLFWwindow);
        glfwTerminate();
    }


    void Window::OnUpdate() {
        glfwSwapBuffers(mGLFWwindow);
        glfwPollEvents();
    }

}
