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
    window_specs.vsync = true;
    OGLR::Window window(window_specs);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    std::unique_ptr<OGLR::Shader> default_shader = std::make_unique<OGLR::Shader>("res/shaders/default.glsl");
    std::unique_ptr<OGLR::Shader> plane_shader = std::make_unique<OGLR::Shader>("res/shaders/bad_reflection.glsl");
    OGLR::Model model(argv[1]);

    model.Rotate(-90, glm::vec3(1.0f, 0.0f, 0.0f));
    model.Scale(glm::vec3(0.01f));

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

    std::vector<float> planeVertices {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    std::vector<uint32_t> planeIndices {
        0, 1, 2,
        2, 3, 0
    };

    OGLR::VertexArray planeVA;
    planeVA.Bind();
    OGLR::VertexBuffer planeVB(planeVertices);
    OGLR::IndexBuffer planeIB(planeIndices);
    OGLR::VertexLayout planeLayout;
    planeLayout.Push<float>(3, false);
    planeLayout.Push<float>(2, false);
    planeVA.AddVertexData(&planeVB, &planeIB, planeLayout);

    glm::mat4 planeModel = glm::mat4(1.0f);
    planeModel = glm::translate(planeModel, glm::vec3(0.0f, -10.0f, 0.0f));
    planeModel = glm::rotate(planeModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    planeModel = glm::scale(planeModel, glm::vec3(30.0f));

    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    uint32_t renderTexture;
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    uint32_t depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);

    uint32_t drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -5;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int32_t orgFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &orgFB);

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
            plane_shader.reset(new OGLR::Shader("res/shaders/bad_reflection.glsl"));
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

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glViewport(0, 0, 1920, 1080);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model.Draw(default_shader.get(), view, proj);
        glGenerateTextureMipmap(renderTexture);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, orgFB);
        glViewport(0, 0, window.GetWidth(), window.GetHeight());
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model.Draw(default_shader.get(), view, proj);

        glm::mat4 mvp = proj * view * planeModel;
        glm::mat4 mv = view * planeModel;
        glm::mat4 mv_norm = glm::transpose(glm::inverse(mv));

        plane_shader->Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        plane_shader->SetUniformMatrix4("mvMatrix", mv);
        plane_shader->SetUniformMatrix4("normalMatrix", mv_norm);
        plane_shader->SetUniformMatrix4("mvp", mvp);
        plane_shader->SetUniform1i("renTexture", 0);

        planeVA.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        planeVA.UnBind();
        glBindTexture(GL_TEXTURE_2D, 0);

        window.OnUpdate();
    }

    return 0;
}
