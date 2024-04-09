#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 light_mat;
uniform mat4 model;
out vec3 vertexPosition;

void main() {
    gl_Position = inverse(light_mat) * model * vec4(aPos, 1.0);
    vec4 vPosition = model * vec4(aPos, 1.0);
    vertexPosition = vPosition.xyz / vPosition.w;
}