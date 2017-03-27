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

struct Material {
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	float shinny;
};

//uniform sampler2D tex0;

uniform Light light;

uniform Material material;

in vec2 o_texCoords;

in mat3 o_tbn;

in vec3 o_lightDir;

in vec3 o_view;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 colorBuffer;

vec3 computeNormal(vec2 tc) {
	vec3 normal;
	normal = texture(material.normal, tc).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(o_tbn * normal);
	return normal;
}
	
void main() {
	vec3 normal = computeNormal(o_texCoords);
	vec3 lightDir = normalize(o_lightDir);
	vec3 view = normalize(o_view);
	vec3 albedo = texture(material.albedo, o_texCoords).xyz;
	vec3 specular_map = texture(material.specular, o_texCoords).xyz;
	
	float ndotl = max(dot(normal, lightDir), 0.0);
	
	vec3 diffuse = albedo * light.diffuse * ndotl;
	vec3 ambient = albedo * light.ambient;
	
	vec3 halfVec = normalize(view + lightDir);
	float ndoth = max(dot(normal, halfVec), 0.0);
	//float shinny = material.shinny * specular_map.r;
	
	vec3 specular = vec3(light.intencity * specular_map.r) * pow(ndoth, material.shinny);
	
	fragColor = vec4(ambient + diffuse + specular, 1.0);
	//fragColor = vec4(normal, 1.0);
	colorBuffer = vec4(specular, 1.0);
}
