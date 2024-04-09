#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
mat4 light_mat;

out vec3 vertexPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = light_mat * model * vec4(aPos, 1.0);
    vec4 vPosition = model * vec4(aPos, 1.0);
    vertexPosition = vPosition.xyz / vPosition.w;
}