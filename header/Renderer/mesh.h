#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>

#include <Renderer/shader.h>
#include <Renderer/vertex_array.h>
#include <Renderer/Texture2D.h>

#include <string>
#include <vector>
#include <memory>

namespace OGLR {

    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture2D>& textures)
            :mVertices(vertices), mIndices(indices), mTextures(textures) {
            mVAO = std::make_unique<VertexArray>();
            mVAO->Bind();
            mVBO = std::make_unique<VertexBuffer>(vertices);
            mEBO = std::make_unique<IndexBuffer>(indices);
            setupMesh();
        }

        void Draw(Shader* shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)  {
            uint32_t diffuseNr  = 1;
            uint32_t specularNr = 1;
            uint32_t normalNr   = 1;
            uint32_t shininessNr = 1;
            
            shader->Bind();
            for(uint32_t i = 0; i < mTextures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                mTextures[i].Bind();
                std::string number;
                std::string name = mTextures[i].GetName();
                if(name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if(name == "texture_specular")
                    number = std::to_string(specularNr++);
                else if(name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if(name == "texture_shininess")
                    number = std::to_string(shininessNr++);

                shader->SetUniform1i(std::string(name + number), i);
            }
            glm::mat4 mvp = proj * view * model;
            glm::mat4 mv = view * model;
            glm::mat4 mv_norm = glm::transpose(glm::inverse(mv));

            shader->SetUniformMatrix4("mvMatrix", mv);
            shader->SetUniformMatrix4("normalMatrix", mv_norm);
            shader->SetUniformMatrix4("mvp", mvp);
            mVAO->Bind();
            glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(mIndices.size()), GL_UNSIGNED_INT, nullptr);
            mVAO->UnBind();
            shader->UnBind();
        }
    private:
        void setupMesh() {
            VertexLayout layout;
            layout.Push<float>(3, false);
            layout.Push<float>(3, false);
            layout.Push<float>(2, false);
            mVAO->AddVertexData(mVBO.get(), mEBO.get(), layout);
        }
    private:
        std::vector<Vertex>       mVertices;
        std::vector<uint32_t> mIndices;
        std::vector<Texture2D>      mTextures;
        std::unique_ptr<VertexArray> mVAO;
        std::unique_ptr<VertexBuffer> mVBO;
        std::unique_ptr<IndexBuffer> mEBO;
    };
    
}
