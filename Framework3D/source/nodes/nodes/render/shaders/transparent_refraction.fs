#version 430 core

uniform vec2 iResolution;

uniform sampler2D baseColor;
uniform sampler2D normalTexture;

uniform sampler2D position;
uniform sampler2D depth;
uniform sampler2D env_texture;
uniform vec3 camPos;

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

float reflectance(float cosine, float refraction_index) {
        // Use Schlick's approximation for reflectance.
	float r0 = (1 - refraction_index) / (1 + refraction_index);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 get_env_color(vec3 dir) {

	vec2 env_texCoords = cylindricalUV(normalize(dir));
	vec3 env_color = texture(env_texture, env_texCoords).xyz;
	return env_color;
}

void main() {
	vec2 uv = gl_FragCoord.xy / iResolution;
	vec3 pos = texture2D(position, uv).xyz;

	vec3 normal = normalize(texture2D(normalTexture, uv).xyz);
	vec3 I = normalize(camPos - pos);

	if(dot(normal, I) < 0)
		normal *= -1;

	float depth_val = texture2D(depth, uv).x;

	if(depth_val != 0) {
		//vec3 R = reflect(I, normalize(normal));

		float eta = 1.5;
		vec3 refract_dir = refract(-I, normal, 1.0 / eta);
		vec3 reflect_dir = reflect(-I, normal);

		float cos_theta = dot(I, normal);
		float reflectance_ = reflectance(cos_theta, eta);

		vec3 refraction_color = get_env_color(refract_dir);
		vec3 reflection_color = get_env_color(reflect_dir);
		vec3 result = mix(refraction_color, reflection_color, reflectance_);

		//result = vec3(reflectance_);
		Color = vec4(result, 1.0);
	}
	//Color = vec4(env_texCoords,1,1);
}
