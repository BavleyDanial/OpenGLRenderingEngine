#include <glfw_window.h>
#include <cassert>
#include <iostream>

#include <glad/glad.h>

namespace OGLR {

    Window::Window(const WindowSpecs& specs)
        :mGLFWwindow(nullptr), mSpecs(specs) {

        if (!glfwInit())
            assert("Couldn't initialise glfw");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        mGLFWwindow = glfwCreateWindow(static_cast<int>(mSpecs.width), static_cast<int>(mSpecs.height), mSpecs.title.c_str(), nullptr, nullptr);
        if (!mGLFWwindow) {
            glfwTerminate();
            assert("Couldn't initialise window");
        }

        glfwMakeContextCurrent(mGLFWwindow);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            assert("Failed to initialize GLAD");
        }

        if (mSpecs.vsync)
            glfwSwapInterval(1);

        if (mSpecs.fullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(mGLFWwindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            mSpecs.width = mode->width;
            mSpecs.height = mode->height;
        }

        glViewport(0, 0, static_cast<int>(mSpecs.width), static_cast<int>(mSpecs.height));
        Input::SetCurrentWindow(mGLFWwindow);

        glfwSetWindowUserPointer(mGLFWwindow, this);
        glfwSetWindowSizeCallback(mGLFWwindow, [](GLFWwindow* window, int width, int height) {
                Window* ogreWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
                ogreWindow->Resize(width, height);
        });

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(Window::DebugLogOGL, nullptr);
    }

    Window::~Window() {
        glfwDestroyWindow(mGLFWwindow);
        glfwTerminate();
    }

   void Window::DebugLogOGL(uint32_t source, uint32_t type, uint32_t id,
                                                    uint32_t severity, int32_t length,
                                                    const char* msg, const void* data) {
           std::string _source;
           std::string _type;
           std::string _severity;

           switch (source) {
               case GL_DEBUG_SOURCE_API:
                   _source = "API";
                   break;

               case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                   _source = "WINDOW SYSTEM";
                   break;

               case GL_DEBUG_SOURCE_SHADER_COMPILER:
                   _source = "SHADER COMPILER";
                   break;

               case GL_DEBUG_SOURCE_THIRD_PARTY:
                   _source = "THIRD PARTY";
                   break;

               case GL_DEBUG_SOURCE_APPLICATION:
                   _source = "APPLICATION";
                   break;

               case GL_DEBUG_SOURCE_OTHER:
                   _source = "UNKNOWN";
                   break;

               default:
                   _source = "UNKNOWN";
                   break;
           }

           switch (type) {
               case GL_DEBUG_TYPE_ERROR:
                   _type = "ERROR";
                   break;

               case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                   _type = "DEPRECATED BEHAVIOR";
                   break;

               case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                   _type = "UDEFINED BEHAVIOR";
                   break;

               case GL_DEBUG_TYPE_PORTABILITY:
                   _type = "PORTABILITY";
                   break;

               case GL_DEBUG_TYPE_PERFORMANCE:
                   _type = "PERFORMANCE";
                   break;

               case GL_DEBUG_TYPE_OTHER:
                   _type = "OTHER";
                   break;

               case GL_DEBUG_TYPE_MARKER:
                   _type = "MARKER";
                   break;

               default:
                   _type = "UNKNOWN";
                   break;
           }

           switch (severity) {
               case GL_DEBUG_SEVERITY_HIGH:
                   _severity = "HIGH";
                   break;

               case GL_DEBUG_SEVERITY_MEDIUM:
                   _severity = "MEDIUM";
                   break;

               case GL_DEBUG_SEVERITY_LOW:
                   _severity = "LOW";
                   break;

               case GL_DEBUG_SEVERITY_NOTIFICATION:
                   _severity = "NOTIFICATION";
                   break;

               default:
                   _severity = "UNKNOWN";
                   break;
        }

        printf("%d: %s of %s severity, raised from %s: %s\n",
                id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
   }

    void Window::Close() const {
        glfwSetWindowShouldClose(mGLFWwindow, true);
    }

    void Window::SetWidth(int width) {
        Resize(width, mSpecs.height);
    }

    void Window::SetHeight(int height) {
        Resize(mSpecs.width, height);
    }

    void Window::Resize(int width, int height) {
        mSpecs.width = width;
        mSpecs.height = height;
        glfwSetWindowSize(mGLFWwindow, static_cast<int>(mSpecs.width), static_cast<int>(mSpecs.height));
        glViewport(0, 0, static_cast<int>(mSpecs.width), static_cast<int>(mSpecs.height));
    }

    void Window::OnUpdate() const {
        glfwSwapBuffers(mGLFWwindow);
        Input::OnUpdate();
    }

}
