/**
	shader.vert
	
	This is the vertex shader for the simple triangle...
	
*/

#version 400 core

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
};

layout(location = 0) in vec3 vertices;

//layout(location = 1) in vec3 colors;

layout(location = 1) in vec2 texCoords;

layout(location = 2) in vec3 normals;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

uniform mat4 normalModel;

uniform vec3 cameraPos;

uniform Light light;

out vec2 o_texCoords;

out vec3 o_color;

void main() {
	gl_Position = projection * view * model * vec4(vertices, 1.0);
	
	o_texCoords = texCoords;
	
	// Handle Lighting
	vec3 normal = normalize(vec3(mat3(normalModel) * normals));
	
	vec3 lightDir = normalize(light.position);
	
	float ndotl = max(dot(normal, lightDir), 0.0);
	
	vec3 diffuse = light.diffuse * ndotl;
	
	vec3 ambient = light.ambient;
	
	o_color = diffuse + ambient;
}
