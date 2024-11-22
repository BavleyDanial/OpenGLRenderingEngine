#pragma once

#include <cstdint>
#include <string>

namespace OGLR {

   struct TextureSpecs {
      std::string path;
      std::string type;
      uint8_t* data;
      uint32_t width, height;
      uint32_t format;
   };
   
   class Texture2D {
   public:
      Texture2D() = default;
      Texture2D(const TextureSpecs& specs);

      void Bind() const;
      void UnBind() const;

      uint32_t GetWidth() const { return mSpecs.width; }
      uint32_t GetHeight() const { return mSpecs.height; }

      std::string GetName() const { return mSpecs.type; }
      std::string GetPath() const { return mSpecs.path; }
   private:
      uint32_t mRendererID;
      TextureSpecs mSpecs;
   };
   
}