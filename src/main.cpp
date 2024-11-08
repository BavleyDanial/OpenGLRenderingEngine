#include <glad/glad.h>

#include <glfw_window.h>
#include <glfw_input.h>

#include <Renderer/vertex_buffer.h>
#include <Renderer/index_buffer.h>
#include <Renderer/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cyCodeBase-master/cyTriMesh.h>

#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>

#include <map>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Program expected 1 argument, recieved " << argc << '\n';
        return -1;
    }

    OGLR::WindowSpecs windowSpecs{};
    windowSpecs.vsync = false;
    windowSpecs.fullscreen = true;
    OGLR::Window window(windowSpecs);

    std::unique_ptr<OGLR::Shader> default_shader = std::make_unique<OGLR::Shader>("res/shaders/default.glsl");
    cy::TriMesh mesh;
    mesh.LoadFromFileObj(argv[1]);

    std::vector<float> buffer_data;
    std::vector<uint32_t> indices;
    std::map<std::pair<int, int>, int> vertexMap;

    for (int i = 0; i < mesh.NF(); i++) {
        for (int j = 0; j < 3; ++j) {
            int pos_index = mesh.F(i).v[j];
            int norm_index = mesh.FN(i).v[j];
            auto key = std::make_pair(pos_index, norm_index);

            if (vertexMap.find(key) == vertexMap.end()) {
                auto position = mesh.V(pos_index);
                auto norm = mesh.VN(norm_index);
                buffer_data.push_back(position[0]);
                buffer_data.push_back(position[1]);
                buffer_data.push_back(position[2]);
                buffer_data.push_back(norm[0]);
                buffer_data.push_back(norm[1]);
                buffer_data.push_back(norm[2]);

                vertexMap[key] = buffer_data.size() / 6 - 1;
            }

            indices.push_back(vertexMap[key]);
        }
    }

    glEnable(GL_DEPTH_TEST);

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    OGLR::VertexBuffer vertex_buff(buffer_data);
    vertex_buff.Bind();
    OGLR::IndexBuffer index_buff(indices);
    index_buff.Bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6*sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    bool point_mode = false;
    bool line_mode = false;
    float point_size = 1.0f;

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), window.GetWidth() / (float)window.GetHeight(), 0.01f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f));

    glm::vec3 cam_pos = glm::vec3(0.0f, 1.0f, 2.5f);
    float delta_time = 0.0f;
    float last_time = 0.0f;

    while (!window.ShouldClose()) {
        float current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);

        if (OGLR::Input::KeyPressed(GLFW_KEY_K))
            glPointSize(++point_size);
        else if (OGLR::Input::KeyPressed(GLFW_KEY_J))
            glPointSize(--point_size);

        if (OGLR::Input::KeyPressed(GLFW_KEY_F))
            point_mode = !point_mode;
        if (OGLR::Input::KeyPressed(GLFW_KEY_3))
            line_mode = !line_mode;

        if (OGLR::Input::KeyHeld(GLFW_KEY_W))
            cam_pos.z -= 1 * 0.0167;
        if (OGLR::Input::KeyHeld(GLFW_KEY_S))
            cam_pos.z += 1 * 0.0167;
        if (OGLR::Input::KeyHeld(GLFW_KEY_D))
            cam_pos.x += 1 * 0.0167;
        if (OGLR::Input::KeyHeld(GLFW_KEY_A))
            cam_pos.x -= 1 * 0.0167;
        if (OGLR::Input::KeyHeld(GLFW_KEY_E))
            cam_pos.y += 1 * 0.0167;
        if (OGLR::Input::KeyHeld(GLFW_KEY_Q))
            cam_pos.y -= 1 * 0.0167;

        if (OGLR::Input::KeyPressed(GLFW_KEY_H)) {
            default_shader.reset(new OGLR::Shader("res/shaders/default.glsl"));
            default_shader->Bind();
        }

        if (point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (line_mode && !point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if (!point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        const float radius = 2.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        //view = glm::lookAt(glm::vec3(camX, 1.7f, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));  
        view = glm::lookAt(cam_pos, cam_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));  
        glm::mat4 mvp = proj * view * model;

        default_shader->Bind();
        default_shader->SetUniformMatrix4("mvp", mvp);
        default_shader->SetUniformMatrix4("model", model);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        window.OnUpdate();
    }

    return 0;
}
