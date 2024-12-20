#shader vertex
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTex;

uniform mat4 mvp;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 texCoord;

void main() {
    fragPosition = vec3(mvMatrix * vec4(inPosition, 1.0f));
    fragNormal = normalize(mat3(normalMatrix) * inNormal); 
    texCoord = inTex;

    gl_Position = mvp * vec4(inPosition, 1.0f);
}


#shader fragment
#version 330 core

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 255

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct DirLight { 
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform int dir_lights_count;
uniform DirLight dir_lights[MAX_DIR_LIGHTS];
uniform int point_lights_count;
uniform PointLight point_lights[MAX_POINT_LIGHTS];

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = -normalize(fragPosition);

    // texture colors
    vec3 diff_color = texture(texture_diffuse1, texCoord).xyz;
    vec3 spec_color = texture(texture_specular1, texCoord).xyz;
    vec3 ambient = 0.15f * diff_color * dir_lights[0].color;

    // Light calculations
    vec3 result = ambient;
    for (int i = 0; i < dir_lights_count; i++) {
        vec3 lightDirection = -normalize(dir_lights[i].direction);
        float geo_term = max(dot(normal, lightDirection), 0.0f);
        vec3 diffuse = geo_term * diff_color;

        vec3 halfVec = normalize(lightDirection + viewDir);
        float spec = pow(max(dot(normal, halfVec), 0.0f), 32);
        vec3 specular = spec * spec_color;

        result += dir_lights[i].intensity * (diffuse + specular);
    };

    for (int i = 0; i < point_lights_count; i++) {
        vec3 lightDirection = -normalize(point_lights[i].position - viewDir);
        float geo_term = max(dot(normal, lightDirection), 0.0f);
        vec3 diffuse = geo_term * diff_color;

        vec3 halfVec = normalize(lightDirection + viewDir);
        float spec = pow(max(dot(normal, halfVec), 0.0f), 32);
        vec3 specular = spec * spec_color;

        result += point_lights[i].intensity * (diffuse + specular);
    };

    fragColor = vec4(result, 1.0f);
}
