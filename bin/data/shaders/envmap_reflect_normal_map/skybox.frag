#version 400 core

uniform samplerCube skybox;

in vec3 o_texCoords;

out vec4 out_Color;

void main() {
	out_Color = texture(skybox, o_texCoords);
}