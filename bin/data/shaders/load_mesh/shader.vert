/**
	shader.vert
	
	This is the vertex shader for the simple triangle...
	
*/

#version 400 core

layout(location = 0) in vec3 vertices;

//layout(location = 1) in vec3 colors;

layout(location = 1) in vec2 texCoords;

layout(location = 2) in vec3 normals;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

out vec2 o_texCoords;

void main() {
	gl_Position = projection * view * model * vec4(vertices, 1.0);
	
	o_texCoords = texCoords;
}
