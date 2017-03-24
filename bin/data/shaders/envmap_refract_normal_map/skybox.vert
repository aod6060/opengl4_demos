#version 400 core

layout(location = 0) in vec3 vertices;

layout(location = 1) in vec2 texCoords;

layout(location = 2) in vec3 normals;

layout(location = 3) in vec3 tangents;

layout(location = 4) in vec3 bitangents;


uniform mat4 projection;

uniform mat4 view;

out vec3 o_texCoords;

void main() {
	mat4 v = view;
	v[3][0] = 0;
	v[3][1] = 0;
	v[3][2] = 0;
	vec4 pos = projection * v * vec4(vertices, 1.0);
	gl_Position = pos.xyww;
	o_texCoords = vertices;
}