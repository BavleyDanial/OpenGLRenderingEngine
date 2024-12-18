#pragma once

#include <Renderer/model.h>

#include <glm/glm.hpp>

namespace OGLR {

    struct DirectionalLight {
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
    };

    struct PointLight {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    };

    // TODO: Add camera and optimize models
    struct Scene {
        std::vector<Model> models;

        // Lights
        std::vector<PointLight> point_lights;
        std::vector<DirectionalLight> directional_lights;
    };
    
}