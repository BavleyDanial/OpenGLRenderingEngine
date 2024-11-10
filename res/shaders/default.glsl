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
    vec4 viewPosition = mvMatrix * vec4(inPosition, 1.0);
    fragPosition = vec3(viewPosition);
    fragNormal = normalize(mat3(normalMatrix) * inNormal); 
    texCoord = inTex;

    gl_Position = mvp * vec4(inPosition, 1.0f);
}


#shader fragment
#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoord;

uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float specular_expo;

uniform vec3 viewPos;      // Camera position in view space
uniform vec3 lightDir;     // Directional light direction in view space
uniform float light_intensity;     // Directional light direction in view space

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 lightDirection = -normalize(lightDir);
    
    vec3 ambient = ambient_color * diffuse_color;
    
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * texture(diffuse_tex, texCoord).xyz;

    vec3 halfVec = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normal, halfVec), 0.0), specular_expo);
    vec3 specular = spec * texture(specular_tex, texCoord).xyz * specular_color;

    vec3 result = ambient + light_intensity * (diffuse + specular);
    fragColor = vec4(result, 1.0f);
}
