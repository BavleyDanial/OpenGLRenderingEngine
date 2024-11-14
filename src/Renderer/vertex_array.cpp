#include <Renderer/vertex_array.h>

#include <iostream>

namespace OGLR
{
        template <typename T>
        void VertexLayout::Push(uint32_t n, bool normalized)
        {
                std::cerr << "this type isn't supported by VertexLayout\n";
        }

        template <>
        void VertexLayout::Push<float>(uint32_t n, bool normalized)
        {
                mElements.push_back({GL_FLOAT, static_cast<uint32_t>(n * sizeof(float)), n, normalized });
                mByteOffset += n * sizeof(float);
        }

        VertexArray::VertexArray()
        {
                glGenVertexArrays(1, &mRendererID);
        }

        VertexArray::~VertexArray()
        {
                glDeleteVertexArrays(1, &mRendererID);
        }

        void VertexArray::Bind() const
        {
                glBindVertexArray(mRendererID);
        }

        void VertexArray::UnBind() const
        {
                glBindVertexArray(0);
        }


        void VertexArray::AddVertexData(const VertexBuffer& vb, const IndexBuffer& ib, const VertexLayout& layout)
        {
                vb.Bind();
                ib.Bind();
                
                int offset = 0;
                int index = 0;
                for (auto& element : layout.GetElements()) {
                        glVertexAttribPointer(index, element.count, element.glType, element.normalized, layout.GetStride(), (void*) offset);
                        glEnableVertexAttribArray(index);
                        offset += element.size;
                        index++;
                }
        }

}
