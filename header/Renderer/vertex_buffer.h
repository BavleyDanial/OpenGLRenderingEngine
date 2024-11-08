#pragma once

#include <cstdint>
#include <vector>

namespace OGLR {

    class VertexBuffer {
    public:
        VertexBuffer() = default;
        VertexBuffer(const std::vector<float>& data);

        void Bind() const;
        void UnBind() const;

    private:
        uint32_t mRendererID;
    };

}
