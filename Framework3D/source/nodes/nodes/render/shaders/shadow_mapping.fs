#version 430 core

uniform mat4 light_view;
uniform mat4 light_projection;
in vec3 vertexPosition;
layout(location = 0) out float shadow_map0;

void main() {
    vec4 clipPos = light_projection * light_view * (vec4(vertexPosition, 1.0));
    shadow_map0 = (clipPos.z / clipPos.w);
}