#include <glad/glad.h>
#include <glfw_window.h>

int main()
{
    OGLR::WindowSpecs windowSpecs{};
    OGLR::Window window(windowSpecs);

    while (!window.ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);

        window.OnUpdate();
    }

    return 0;
}
