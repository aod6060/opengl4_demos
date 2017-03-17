/**
	shader.vert
	
	This is the vertex shader for the simple triangle...
	
*/

#version 400 core

layout(location = 0) in vec3 vertices;

void main() {
	gl_Position = vec4(vertices, 1.0);
}
