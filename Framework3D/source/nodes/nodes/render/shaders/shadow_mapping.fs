#version 430

uniform mat4 light_mat;

in vec3 vertexPosition;
layout(location = 0) out float shadow_map0;

void main() {
vec4 clipPos = light_mat * (vec4(vertexPosition, 1.0));
shadow_map0 = clipPos.z / clipPos.w;
shadow_map0 = 1.0;
}