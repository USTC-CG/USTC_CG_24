#version 430 core

// Define a uniform struct for lights
struct Light {
    vec4 position;
    vec4 color;
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
Color = vec4(texture(shadow_maps, vec3(uv,0)));
//Color = texture2D(position,uv);
//Color = lights[0].position;
}