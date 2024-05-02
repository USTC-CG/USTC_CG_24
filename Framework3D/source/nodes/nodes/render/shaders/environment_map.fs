#version 430 core

uniform vec2 iResolution;

uniform sampler2D color;
uniform sampler2D depth;
uniform sampler2D env_texture;
uniform mat4 projection;
uniform mat4 view;

layout(location = 0) out vec4 Color;

// Constants
const float PI = 3.14159265359;
const float TWO_PI = 6.2831853071;

// Calculate cylindrical texture coordinates
vec2 cylindricalUV(vec3 dir) {
	float theta = atan(dir.y, dir.x); // Angle around the cylinder
	float phi = 0.5 * (-dir.z) + 0.5; // Height of the cylinder
	return vec2((PI + theta) / TWO_PI, phi);
}

void main() {
	vec2 uv = gl_FragCoord.xy / iResolution;
	float depth_val = texture2D(depth, uv).x;

	if(depth_val == 0) {
		vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
		mat4 view_mat = mat4(mat3(view));

    // Calculate world space position from clip space position
		vec4 worldSpacePos = inverse(projection * view_mat) * clipSpacePos;
		vec3 position = worldSpacePos.xyz;

    // Calculate cylindrical texture coordinates
		vec2 env_texCoords = cylindricalUV(normalize(position));
		vec3 env_color = texture(env_texture, env_texCoords).xyz;
		Color = vec4(env_color, 1.0);
	} else {
		vec3 color_val = texture(color, uv).xyz;

		Color = vec4(color_val, 1.0);
	}
	//Color = vec4(env_texCoords,1,1);
}
