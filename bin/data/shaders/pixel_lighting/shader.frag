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
/*
struct Material {
	sampler2D albedo;
	float shinny;
};
*/
uniform sampler2D tex0;

uniform Light light;

//uniform Material material;

in vec2 o_texCoords;

in vec3 o_normal;

in vec3 o_lightDir;

in vec3 o_view;

out vec4 fragColor;
	
void main() {

	vec3 normal = normalize(o_normal);
	vec3 lightDir = normalize(o_lightDir);
	vec3 view = normalize(o_view);
	vec3 samp = texture(tex0, o_texCoords).xyz;
	
	float ndotl = max(dot(normal, lightDir), 0.0);
	
	vec3 diffuse = samp * light.diffuse * ndotl;
	vec3 ambient = samp * light.ambient;
	
	vec3 halfVec = normalize(view + lightDir);
	float ndoth = max(dot(normal, halfVec), 0.0);
	vec3 specular = vec3(light.intencity) * pow(ndoth, 64.0);
	
	fragColor = vec4(ambient + diffuse + specular, 1.0);
}
