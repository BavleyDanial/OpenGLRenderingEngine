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

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0f);
}
