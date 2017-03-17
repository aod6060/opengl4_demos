/**
	shader.frag
	
	This is the fragment shader for our simple triangle...
*/

#version 400 core

in vec3 o_colors;

out vec4 fragColor;


void main() {
	fragColor = vec4(o_colors, 1.0);
}
