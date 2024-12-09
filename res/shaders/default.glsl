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

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 lightDir;     // Directional light direction in view space
uniform float light_intensity;     // Directional light direction in view space

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = -normalize(fragPosition);
    vec3 lightDirection = -normalize(lightDir);

    vec3 color = texture(texture_diffuse1, texCoord).xyz;
    vec3 ambient = 0.15f * color;

    float diff = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = diff * color;

    vec3 halfVec = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normal, halfVec), 0.0f), 32);
    vec3 specular = spec * texture(texture_specular1, texCoord).xyz;

    vec3 result = ambient + light_intensity * (diffuse + specular);
    fragColor = vec4(result, 1.0f);
}
