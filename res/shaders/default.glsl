#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorms;

uniform mat4 mvp;
uniform mat4 model;

out vec4 vColor;

void main() {
    gl_Position = mvp * vec4(aPos, 1.0f);
    vColor = model * vec4(aNorms, 1.0f);
}

#shader fragment
#version 330 core

out vec4 FragColor;
in vec4 vColor;

void main() {
   FragColor = vec4(normalize(vColor.xyz), 1.0f);
}
