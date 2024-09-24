#include <glad/glad.h>
#include <glfw_window.h>
#include <glfw_input.h>

#include <iostream>

int main()
{
    OGLR::WindowSpecs windowSpecs{};
    OGLR::Window window(windowSpecs);

    while (!window.ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);

        if (OGLR::Input::KeyPressed(GLFW_KEY_ESCAPE))
            window.Close();

        if (OGLR::Input::KeyPressed(GLFW_KEY_F))
            std::cout << "Pressed F\n";
        if (OGLR::Input::KeyReleased(GLFW_KEY_F))
            std::cout << "Released F\n";

        window.OnUpdate();
    }

    return 0;
}
