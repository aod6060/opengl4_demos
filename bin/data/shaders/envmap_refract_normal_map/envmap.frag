/**
	shader.frag
	
	This is the fragment shader for our simple triangle...
*/

#version 400 core

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
	float intencity;
};

uniform samplerCube skybox;

uniform sampler2D normal_map;

uniform Light light;

uniform vec3 cameraPos;

in vec3 o_normal;

in vec3 o_position;
in vec3 o_view;
in vec3 o_lightDir;
in mat3 o_tbn;
in vec2 o_texCoord;

out vec4 fragColor;

vec3 computeNormal(vec2 tc) {
	vec3 normal;
	normal = texture(normal_map, tc).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(o_tbn * normal);
	return normal;
}

void main() {
	vec3 normal = normalize(o_normal);
	vec3 n2 = computeNormal(o_texCoord);
	vec3 lightDir = normalize(o_lightDir);
	vec3 view = normalize(o_view);
	// Reflection
	float ratio = 1 / 1.52;
	vec3 I = normalize(o_position - cameraPos);
	vec3 R = refract(I, n2, ratio);
	vec3 albedo = texture(skybox, R).xyz;
	// Handle Lighting
	// Diffuse
	float ndotl = max(dot(n2, lightDir), 0.0);
	vec3 diffuse = albedo * light.diffuse * ndotl;
	// Ambient
	vec3 ambient = albedo * light.ambient;
	// Specular
	vec3 halfVec = normalize(view + lightDir);
	float ndoth = max(dot(n2, halfVec), 0.0);
	vec3 specular = albedo * light.intencity * pow(ndoth, 128.0);
	// Finish Fragment
	fragColor = vec4(ambient + diffuse + specular, 1.0);
	//fragColor = vec4(n2, 1.0);
}
