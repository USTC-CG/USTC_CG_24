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

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D metallic_roughness;
uniform sampler2D diffuseColor;
uniform sampler2D shadow_maps;

uniform int light_count;

layout(location = 0) out vec4 Color;

void main() {
vec2 uv = gl_FragCoord.xy / iResolution;
Color = texture2D(diffuseColor, uv);
Color = vec4(1.0,1,1.0, 1.0);

}