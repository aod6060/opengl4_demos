/**
	shader.frag
	
	This is the fragment shader for our simple triangle...
*/

#version 400 core

#define NUM_LIGHTS 4
#define NUM_LIGHTS_AVG 0.25

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
	float intencity;
	// Attenuation
	float constant;
	float linear;
	float quad;
};

struct Material {
	sampler2D albedo;
	sampler2D specular;
	sampler2D normal;
	float shinny;
};

//uniform Light light;
uniform Light lights[NUM_LIGHTS];

uniform Material material;

in vec2 o_texCoords;

in vec3 o_normal;

in mat3 o_tbn;

in vec3 o_lightDir[NUM_LIGHTS];

in vec3 o_view;

in vec3 o_fragPos;

out vec4 fragColor;

vec3 computeNormal(vec2 tc) {
	vec3 normal;
	normal = texture(material.normal, tc).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(o_tbn * normal);
	return normal;
}
	
void main() {
	vec3 fragPos = o_fragPos;
	
	vec3 normal = computeNormal(o_texCoords);
	//vec3 normal = normalize(o_normal);
	
	vec3 view = normalize(o_view);
	vec3 albedo = texture(material.albedo, o_texCoords).xyz;
	vec3 specular_map = texture(material.specular, o_texCoords).xyz;
	
	vec3 color = vec3(0.0);
	
	for(int i = 0; i < NUM_LIGHTS; i++) {
		vec3 lightDir = normalize(o_lightDir[i]);
		float distance = length(lights[i].position - o_fragPos);
		float attenuation = 1.0f / (lights[i].constant + lights[i].linear * distance + lights[i].quad * (distance * distance));
		
		float ndotl = max(dot(normal, lightDir), 0.0);
		
		vec3 diffuse = albedo * lights[i].diffuse * ndotl;
		vec3 ambient = albedo * lights[i].ambient;
		
		vec3 halfVec = normalize(view + lightDir);
		float ndoth = max(dot(normal, halfVec), 0.0);
		//float shinny = material.shinny * specular_map.r;
		
		vec3 specular = vec3(lights[i].intencity * specular_map.r) * pow(ndoth, material.shinny);
		
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
		color += ambient + diffuse + specular;
	}
	
	color = color * NUM_LIGHTS_AVG;
	
	fragColor = vec4(color, 1.0);
	//fragColor = vec4(fragPos, 1.0);
}
