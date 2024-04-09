#version 430

layout(location = 0) out float shadow_map0;

layout(binding = 0) buffer lightsBuffer {
mat4 light_projection[4];
};

in vec3 vertexPosition;

uniform mat4 light_mat;

void main() {
vec4 clipPos = light_mat * (vec4(vertexPosition, 1.0));
shadow_map0 = clipPos.z / clipPos.w;
shadow_map0 =0.5f;
}