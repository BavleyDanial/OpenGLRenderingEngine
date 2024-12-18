#include <Renderer/Texture2D.h>
#include <glad/glad.h>

namespace OGLR {

      Texture2D::Texture2D(const uint8_t* data, const TextureSpecs& specs)
      :mSpecs(specs) {
            glGenTextures(1, &mRendererID);
            glBindTexture(GL_TEXTURE_2D, mRendererID);
            glTexImage2D(GL_TEXTURE_2D, 0, specs.format, specs.width, specs.height, 0, specs.format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      void Texture2D::Bind() const {
            glBindTexture(GL_TEXTURE_2D, mRendererID);
      }

      void Texture2D::UnBind() const {
            glBindTexture(GL_TEXTURE_2D, 0);
      }

}
