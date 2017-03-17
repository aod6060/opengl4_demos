/**
	shader.vert
	
	This is the vertex shader for the simple triangle...
	
*/

#version 400 core

layout(location = 0) in vec3 vertices;

layout(location = 1) in vec3 colors;

layout(location = 2) in vec2 texCoords;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

out vec3 o_colors;

out vec2 o_texCoords;

void main() {
	gl_Position = projection * view * model * vec4(vertices, 1.0);
	
	o_colors = colors;
	
	o_texCoords = texCoords;
}
