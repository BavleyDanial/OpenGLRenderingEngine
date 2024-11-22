#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"

#include <glad/glad.h>

#include <cstdint>

namespace OGLR {

    class VertexLayout {
    private:
        struct Element {
            uint16_t glType;
            uint32_t size;
            uint32_t count;
            bool normalized;
        };
    public:
        VertexLayout() = default;

        template <typename T>
        void Push(uint32_t n, bool normalized);
        
        std::vector<Element> GetElements() const { return mElements; }
        int GetStride() const { return mByteOffset; }
    private:
        std::vector<Element> mElements;
        int mByteOffset = 0;
    };
    
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void AddVertexData(const VertexBuffer* vb, const IndexBuffer* ib, const VertexLayout& layout);

        void Bind() const;
        void UnBind() const;
    private:
        uint32_t mRendererID;
    };
    
}