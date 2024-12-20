#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace OGLR {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };

    class VertexBuffer {
    public:
        VertexBuffer() = default;
        VertexBuffer(const std::vector<Vertex>& bufferData);
        VertexBuffer(const std::vector<float>& bufferData);

        void Bind() const;
        void UnBind() const;

    private:
        uint32_t mRendererID;
    };

}
