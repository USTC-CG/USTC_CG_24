#version 430 core

// Define a uniform struct for lights
struct Light {
    // The matrices are used for shadow mapping. You need to fill it according to how we are filling it when building the normal maps (node_render_shadow_mapping.cpp). 
    // Now, they are filled with identity matrix. You need to modify C++ code innode_render_deferred_lighting.cpp.
    mat4 light_projection;
    mat4 light_view;
    vec4 position;
    vec4 color;
    int shadow_map_id;
};

layout(binding = 0) buffer lightsBuffer {
Light lights[4];
};

uniform vec2 iResolution;

uniform sampler2D diffuseColorSampler;
uniform sampler2D normalMapSampler;
uniform sampler2D metallicRoughnessSampler;
uniform sampler2DArray shadow_maps;
uniform sampler2D position;

uniform int light_count;

layout(location = 0) out vec4 Color;

void main() {
vec2 uv = gl_FragCoord.xy / iResolution;

for(int i = 0;
i < light_count;
i ++) {

float shadow_map_value = texture(shadow_maps, vec3(uv, lights[i].shadow_map_id)).x;

// Visualization of shadow map
Color += vec4(shadow_map_value, 0, 0, 1);
}

}