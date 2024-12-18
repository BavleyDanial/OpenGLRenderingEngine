#include <glad/glad.h>

#include <glfw_window.h>
#include <glfw_input.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Renderer/shader.h>
#include <scene.h>

#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Program expected 1 argument, received " << argc << '\n';
        return -1;
    }

    OGLR::WindowSpecs window_specs{};
    window_specs.fullscreen = false;
    window_specs.vsync = true;
    window_specs.width = 1920;
    window_specs.height = 1080;
    OGLR::Window window(window_specs);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    std::unique_ptr<OGLR::Shader> default_shader = std::make_unique<OGLR::Shader>("res/shaders/default.glsl");
    OGLR::Model model(argv[1]);
    OGLR::Model model2(argv[1]);

    model.Translate(glm::vec3(-20, 0, 0));
    model.Rotate(-90, glm::vec3(1.0f, 0.0f, 0.0f));
    model.Scale(glm::vec3(0.01f));

    model2.Translate(glm::vec3(20, 0, 0));
    model2.Rotate(-90, glm::vec3(1.0f, 0.0f, 0.0f));
    model2.Scale(glm::vec3(0.01));

    OGLR::PointLight point_light;
    point_light.position = glm::vec3(0);
    point_light.color = glm::vec3(1);
    point_light.intensity = 1;

    bool point_mode = false;
    bool line_mode = false;
    float point_size = 1.0f;

    glm::mat4 proj = glm::perspective(glm::radians(60.0f),
        static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()),
        0.01f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);

    OGLR::DirectionalLight dir_light;
    dir_light.direction = glm::vec3(-1.0f, -1.0f, 0.0f);
    dir_light.color = glm::vec3(1.0f);
    dir_light.intensity = 1.0f;

    float lastX = static_cast<float>(window.GetWidth()) / 2;
    float lastY = static_cast<float>(window.GetHeight()) / 2;
    float sens = 0.1f;

    glm::vec3 cam_pos = glm::vec3(0.0f, 1.0f, 2.5f);
    glm::vec3 cam_dir = cam_pos + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_front = glm::normalize(cam_dir);
    glm::vec3 cam_right = glm::cross(cam_front, glm::vec3(0.0f, 1.0f, 0.0f));

    float delta_time = 0.0f;
    float last_time = 0.0f;

    float yaw = -90.0f, pitch = 0.0f;
    OGLR::Input::LockMouse();

    while (!window.ShouldClose()) {
        float current_time = static_cast<float>(glfwGetTime());
        delta_time = current_time - last_time;
        last_time = current_time;

        if (OGLR::Input::KeyPressed(GLFW_KEY_P))
            OGLR::Input::UnLockMouse();
        if (OGLR::Input::KeyPressed(GLFW_KEY_U))
            OGLR::Input::LockMouse();

        if (OGLR::Input::KeyPressed(GLFW_KEY_K))
            glPointSize(++point_size);
        else if (OGLR::Input::KeyPressed(GLFW_KEY_J))
            glPointSize(--point_size);

        if (OGLR::Input::KeyPressed(GLFW_KEY_F))
            point_mode = !point_mode;
        if (OGLR::Input::KeyPressed(GLFW_KEY_3))
            line_mode = !line_mode;

        if (OGLR::Input::KeyHeld(GLFW_KEY_W))
            cam_pos += cam_front * 12.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_S))
            cam_pos -= cam_front * 12.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_D))
            cam_pos += cam_right * 12.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_A))
            cam_pos -= cam_right * 12.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_E))
            cam_pos.y += 12 * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_Q))
            cam_pos.y -= 12 * delta_time;

        if (OGLR::Input::KeyPressed(GLFW_KEY_H)) {
            default_shader.reset(new OGLR::Shader("res/shaders/default.glsl"));
        }

        if (OGLR::Input::KeyHeld(GLFW_KEY_UP))
            dir_light.intensity += 1.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_DOWN))
            dir_light.intensity -= 1.0f * delta_time;
        dir_light.intensity = glm::max(0.0f, dir_light.intensity);

        if (OGLR::Input::IsMouseLocked()) {
            auto[mouseX, mouseY] = OGLR::Input::GetMousePosition();
            float xMouseOffset = mouseX - lastX;
            float yMouseOffset = lastY - mouseY;
            lastX = mouseX; lastY = mouseY;
            xMouseOffset *= sens; yMouseOffset *= sens;

            yaw += xMouseOffset;
            pitch += yMouseOffset;

            if(pitch > 89.0f)
                pitch =  89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;

            cam_dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            cam_dir.y = sin(glm::radians(pitch));
            cam_dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cam_front = glm::normalize(cam_dir);
            cam_right = glm::cross(cam_front, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);

        if (point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (line_mode && !point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if (!point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        view = glm::lookAt(cam_pos, cam_pos + cam_front, glm::vec3(0.0, 1.0, 0.0));  

        default_shader->Bind();
        default_shader->SetUniform3f("dir_lights[0].direction", glm::normalize(glm::mat3(view) * glm::normalize(dir_light.direction)));
        default_shader->SetUniform3f("dir_lights[0].color", dir_light.color);
        default_shader->SetUniform1f("dir_lights[0].intensity", dir_light.intensity);
        default_shader->SetUniform1i("dir_lights_count", 1);

        model.Draw(default_shader.get(), view, proj);
        model2.Draw(default_shader.get(), view, proj);
        window.OnUpdate();
    }

    return 0;
}
