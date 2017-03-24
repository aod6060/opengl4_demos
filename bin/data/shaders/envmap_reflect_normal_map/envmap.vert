/**
	shader.vert
	
	This is the vertex shader for the simple triangle...
	
*/

#version 400 core

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
	float intencity;
};

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 tangents;
layout(location = 4) in vec3 bitangents;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

uniform mat4 normalModel;

uniform vec3 cameraPos;

uniform Light light;

out vec3 o_normal;
out vec3 o_position;
out vec3 o_view;
out vec3 o_lightDir;
out mat3 o_tbn;
out vec2 o_texCoord;

void main() {
	gl_Position = projection * view * model * vec4(vertices, 1.0);
	
	o_normal = mat3(normalModel) * normals;
	vec3 t = normalize(vec3(normalModel * vec4(tangents, 0.0)));
	vec3 b = normalize(vec3(normalModel * vec4(bitangents, 0.0)));
	vec3 n = normalize(vec3(normalModel * vec4(normals, 0.0)));
	o_tbn = mat3(t, b, n);
	
	o_position = vec3(model * vec4(vertices, 0.0));
	vec3 posM = (model * vec4(vertices, 1.0)).xyz;
	o_view = normalize(cameraPos - posM);
	o_lightDir = normalize(-light.position);
	
	// Texture Coord
	o_texCoord = texCoords;
}
