#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <optional>

namespace OGLR {

    struct ShaderSource {
        std::optional<std::string> vertex;
        std::optional<std::string> fragment;
        std::optional<std::string> geometry;
        std::optional<std::string> compute;
    };

    enum class ShaderType {
        UNKNOWN = 0,
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE
    };

    class Shader {
    public:
        Shader() = default;
        Shader(const std::string& filepath);
       ~Shader();

       void Bind();
       void UnBind();

       void SetUniform4f(const std::string& name, const glm::vec4& value);
       void SetUniform3f(const std::string& name, const glm::vec3& value);
       void SetUniformMatrix4(const std::string& name, const glm::mat4& value);
    private:
        int GetUniformLocation(const std::string& name);
    private:
        const ShaderSource ParseShader();
        uint32_t CompileAndLinkShader(const std::string& source, uint32_t type);
    private:
        uint32_t mRendererID;
        std::string mFilePath;
    };

}
