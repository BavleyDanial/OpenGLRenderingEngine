#include <Renderer/vertex_buffer.h>
#include <glad/glad.h>

namespace OGLR {

    VertexBuffer::VertexBuffer(const std::vector<Vertex>& buffer_data) {
        glGenBuffers(1, &mRendererID);
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*buffer_data.size(), buffer_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VertexBuffer::VertexBuffer(const std::vector<float>& bufferData) {
        glGenBuffers(1, &mRendererID);
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*bufferData.size(), bufferData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
    }

    void VertexBuffer::UnBind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}
