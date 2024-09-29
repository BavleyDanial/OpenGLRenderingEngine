#include <glad/glad.h>

#include <glfw_window.h>
#include <glfw_input.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cyCodeBase-master/cyTriMesh.h>

#include <cstdint>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Program expected 1 argument, recieved " << argc << '\n';
        return -1;
    }

    OGLR::WindowSpecs windowSpecs{};
    windowSpecs.fullscreen = true;
    OGLR::Window window(windowSpecs);

    const char* vertex_shader ="#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 mvp;\n"
        "out vec3 vColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = mvp * vec4(aPos, 1.0f);\n"
        "}\0";

    const char* fragment_shader = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\n\0";

    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    glCompileShader(vs);

    uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);

    uint32_t program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    cy::TriMesh mesh;
    mesh.LoadFromFileObj(argv[1]);

    std::vector<float> buffer_data;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh.NF(); i++) {
        indices.push_back(mesh.F(i).v[0]);
        indices.push_back(mesh.F(i).v[1]);
        indices.push_back(mesh.F(i).v[2]);
    }

    for (int i = 0; i < mesh.NV(); i++) {
        buffer_data.push_back(mesh.V(i)[0]);
        buffer_data.push_back(mesh.V(i)[1]);
        buffer_data.push_back(mesh.V(i)[2]);
    }

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vb;
    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buffer_data.size(), buffer_data.data(), GL_STATIC_DRAW);

    uint32_t ib;
    glCreateBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*) 0);
    glEnableVertexAttribArray(0);

    bool point_mode = false;
    float point_size = 1.0f;

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), window.GetWidth() / (float)window.GetHeight(), 0.01f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 cam_pos = glm::vec3(0.0f);
    while (!window.ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(35.0f/255, 35.0f/255, 35.0f/255, 1);

        if (OGLR::Input::KeyPressed(GLFW_KEY_K))
            glPointSize(++point_size);
        else if (OGLR::Input::KeyPressed(GLFW_KEY_J))
            glPointSize(--point_size);

        if (OGLR::Input::KeyPressed(GLFW_KEY_F))
            point_mode = !point_mode;

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

        if (point_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        view = glm::lookAt(cam_pos, cam_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;

        glBindVertexArray(vao);
        glUseProgram(program);
        uint32_t loc = glGetUniformLocation(program, "mvp");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(mvp));

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        window.OnUpdate();
    }

    return 0;
}
