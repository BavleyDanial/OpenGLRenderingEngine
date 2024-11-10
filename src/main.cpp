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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
#include <map>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Program expected 1 argument, received " << argc << '\n';
        return -1;
    }

    OGLR::WindowSpecs windowSpecs{};
	windowSpecs.fullscreen = false;
    windowSpecs.width = 1920;
    windowSpecs.height = 1080;
    OGLR::Window window(windowSpecs);

    std::unique_ptr<OGLR::Shader> default_shader = std::make_unique<OGLR::Shader>("res/shaders/default.glsl");
    cy::TriMesh mesh;
    mesh.LoadFromFileObj(argv[1]);

    std::vector<float> buffer_data;
    std::vector<uint32_t> indices;
    std::map<std::tuple<int, int, int>, int> vertexMap;

    for (uint32_t i = 0; i < mesh.NF(); i++) {
        for (uint32_t j = 0; j < 3; ++j) {
            uint32_t pos_index = mesh.F(static_cast<int>(i)).v[j];
            uint32_t norm_index = mesh.FN(static_cast<int>(i)).v[j];
            uint32_t tex_index = mesh.FT(static_cast<int>(i)).v[j];
            auto key = std::make_tuple(pos_index, norm_index, tex_index);

            if (!vertexMap.contains(key)) {
                auto position = mesh.V(static_cast<int>(pos_index));
                auto norm = mesh.VN(static_cast<int>(norm_index));
                auto tex = mesh.VT(static_cast<int>(tex_index));
                buffer_data.push_back(position[0]);
                buffer_data.push_back(position[1]);
                buffer_data.push_back(position[2]);
                buffer_data.push_back(norm[0]);
                buffer_data.push_back(norm[1]);
                buffer_data.push_back(norm[2]);
                buffer_data.push_back(tex[0]);
                buffer_data.push_back(tex[1]);

                vertexMap[key] = static_cast<int>(buffer_data.size() / 8 - 1);
            }

            indices.push_back(vertexMap[key]);
        }
    }

    struct Texture {
        uint32_t id = 0;
        uint8_t* data = nullptr;
        int width, height, components = 0;
    };
    
    Texture lTex;
    std::string tex_path = std::string("res/") + std::string(mesh.M(0).map_Kd.data);
    lTex.data = stbi_load(tex_path.c_str(), &lTex.width, &lTex.height, &lTex.components, 0);
    
    Texture sTex;
    tex_path = std::string("res/") + std::string(mesh.M(0).map_Ks.data);
    sTex.data = stbi_load(tex_path.c_str(), &sTex.width, &sTex.height, &sTex.components, 0);
    
	std::cout << "Vertices: " << mesh.NV() << " Triangles: " << mesh.NF() << "\n";

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    OGLR::VertexBuffer vertex_buff(buffer_data);
    vertex_buff.Bind();
    OGLR::IndexBuffer index_buff(indices);
    index_buff.Bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8*sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &lTex.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lTex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    lTex.width, lTex.height, 0,
                    GL_RGB, GL_UNSIGNED_BYTE, lTex.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenTextures(1, &sTex.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sTex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    sTex.width, sTex.height, 0,
                    GL_RGB, GL_UNSIGNED_BYTE, sTex.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bool point_mode = false;
    bool line_mode = false;
    float point_size = 1.0f;

    glm::mat4 proj = glm::perspective(glm::radians(60.0f),
        static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()),
        0.01f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f));

    glm::vec3 light_dir = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 lambert = glm::vec3(1.0f, 0.0f, 0.0f);
    float light_intensity = 1.0f;

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
    window.LockMouse();
    
    while (!window.ShouldClose()) {
        float current_time = static_cast<float>(glfwGetTime());
        delta_time = current_time - last_time;
        last_time = current_time;

        if (OGLR::Input::KeyPressed(GLFW_KEY_P))
            window.LockMouse();
        if (OGLR::Input::KeyPressed(GLFW_KEY_U))
            window.UnLockMouse();

        if (OGLR::Input::KeyPressed(GLFW_KEY_K))
            glPointSize(++point_size);
        else if (OGLR::Input::KeyPressed(GLFW_KEY_J))
            glPointSize(--point_size);

        if (OGLR::Input::KeyPressed(GLFW_KEY_F))
            point_mode = !point_mode;
        if (OGLR::Input::KeyPressed(GLFW_KEY_3))
            line_mode = !line_mode;

        if (OGLR::Input::KeyHeld(GLFW_KEY_W))
            cam_pos += cam_front * 3.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_S))
            cam_pos -= cam_front * 3.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_D))
            cam_pos += cam_right * 3.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_A))
            cam_pos -= cam_right * 3.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_E))
            cam_pos.y += 3 * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_Q))
            cam_pos.y -= 3 * delta_time;

        if (OGLR::Input::KeyPressed(GLFW_KEY_H)) {
            default_shader.reset(new OGLR::Shader("res/shaders/default.glsl"));
            default_shader->Bind();
        }

        if (OGLR::Input::KeyHeld(GLFW_KEY_UP))
            light_intensity += 1.0f * delta_time;
        if (OGLR::Input::KeyHeld(GLFW_KEY_DOWN))
            light_intensity -= 1.0f * delta_time;
        light_intensity = glm::max(0.0f, light_intensity);

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

        //constexpr float radius = 4.0f;
        //float camX = static_cast<float>(sin(glfwGetTime())) * radius;
        //float camZ = static_cast<float>(cos(glfwGetTime())) * radius;
        //view = glm::lookAt(glm::vec3(camX, 4.0f, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));  
        view = glm::lookAt(cam_pos, cam_pos + cam_front, glm::vec3(0.0, 1.0, 0.0));  
        glm::mat4 mvp = proj * view * model;
        glm::mat4 mv = view * model;
        glm::mat4 mv_norm = glm::transpose(glm::inverse(mv));

        default_shader->Bind();
        default_shader->SetUniformMatrix4("mvp", mvp);
        default_shader->SetUniformMatrix4("mvMatrix", mv);
        default_shader->SetUniformMatrix4("normalMatrix", mv_norm);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lTex.id); 
        default_shader->SetUniform1i("tex", 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sTex.id); 
        default_shader->SetUniform1i("spec_tex", 1);
        
        default_shader->SetUniform3f("viewPos", cam_pos);
        default_shader->SetUniform3f("lightDir", glm::normalize(glm::mat3(view) * glm::normalize(light_dir)));
        default_shader->SetUniform1f("light_intensity", light_intensity);

        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        window.OnUpdate();
    }

    return 0;
}
