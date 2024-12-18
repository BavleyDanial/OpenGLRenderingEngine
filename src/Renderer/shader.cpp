#include <Renderer/shader.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace OGLR {

    Shader::Shader(const std::string& filepath)
    :mFilePath(filepath), mRendererID(0) {
        ShaderSource source = ParseShader();
        uint32_t vertexID = 0;
        uint32_t fragmentID = 0;
        uint32_t geometryID = 0;
        uint32_t computeID = 0;

        mRendererID = glCreateProgram();

        if (source.vertex.has_value())
            vertexID = CompileAndLinkShader(source.vertex.value(), GL_VERTEX_SHADER);
        if (source.fragment.has_value())
            fragmentID = CompileAndLinkShader(source.fragment.value(), GL_FRAGMENT_SHADER);
        if (source.geometry.has_value())
            geometryID = CompileAndLinkShader(source.geometry.value(), GL_GEOMETRY_SHADER);
        if (source.compute.has_value())
            computeID = CompileAndLinkShader(source.compute.value(), GL_COMPUTE_SHADER);

        glLinkProgram(mRendererID);
        glValidateProgram(mRendererID);

        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        glDeleteShader(geometryID);
        glDeleteShader(computeID);
    }

    Shader::~Shader() {
        glUseProgram(0);
        glDeleteProgram(mRendererID);
    }


    void Shader::Bind() {
        glUseProgram(mRendererID);
    }

    void Shader::UnBind() {
        glUseProgram(0);
    }

    void Shader::SetUniform4f(const std::string& name, const glm::vec4& value) {
        int location = GetUniformLocation(name);
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::SetUniform3f(const std::string& name, const glm::vec3& value) {
        int location = GetUniformLocation(name);
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::SetUniform1f(const std::string& name, float value) {
        int location = GetUniformLocation(name);
        glUniform1f(location, value);
    }

    void Shader::SetUniform1i(const std::string& name, int value) {
        int location = GetUniformLocation(name);
        glUniform1i(location, value);
    }

    void Shader::SetUniformMatrix4(const std::string& name, const glm::mat4& value) {
        int location = GetUniformLocation(name);
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
    }

    void Shader::SetUniformMatrix3(const std::string& name, const glm::mat3& value) {
        int location = GetUniformLocation(name);
        glUniformMatrix3fv(location, 1, false, glm::value_ptr(value));
    }

    int Shader::GetUniformLocation(const std::string& name) {
        int location = glGetUniformLocation(mRendererID, name.c_str());
        return location;
    }

    const ShaderSource Shader::ParseShader() {
        std::ifstream input_file(mFilePath);
        std::string line;

        ShaderType type = ShaderType::UNKNOWN;
        // The following are used to store the source code of each shader type, it's not pretty but it works.
        std::stringstream vertexSS;
        std::stringstream fragmentSS;
        std::stringstream geometrySS;
        std::stringstream computeSS;

        while (std::getline(input_file, line)) {
            // If the line has the '#shader' figure out which stringstream we will write to
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
                else
                    type = ShaderType::UNKNOWN;

                continue;
            }
            // Write to the stringstream of the current shader type source code
            switch(type) {
                case ShaderType::VERTEX:
                    vertexSS << line << "\n";
                    break;
                case ShaderType::FRAGMENT:
                    fragmentSS << line << "\n";
                    break;
                default:
                    std::cout << "We don't support that shader yet" << std::endl;
            }
        }
        // If the string of the source code of a type is not empty store it and return it
        ShaderSource shader_src;
        if (!vertexSS.str().empty())
            shader_src.vertex = vertexSS.str();
        if (!fragmentSS.str().empty())
            shader_src.fragment = fragmentSS.str();
        if (!geometrySS.str().empty())
            shader_src.geometry = geometrySS.str();
        if (!computeSS.str().empty())
            shader_src.compute = computeSS.str();
        return shader_src;
    }


    uint32_t Shader::CompileAndLinkShader(const std::string& source, uint32_t type) {
        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glAttachShader(mRendererID, id);

        // check for linking errors
        glGetShaderiv(mRendererID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(mRendererID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        return id;
    }
}
