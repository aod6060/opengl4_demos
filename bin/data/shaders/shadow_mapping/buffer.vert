#version 400 core

layout(location = 0) in vec3 vertices;

layout(location = 1) in vec2 texCoords;

layout(location = 2) in vec3 normals;

layout(location = 3) in vec3 tangents;

layout(location = 4) in vec3 bitangents;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(vertices, 1.0);
}