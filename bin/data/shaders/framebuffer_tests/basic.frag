/**
	shader.frag
	
	This is the fragment shader for our simple triangle...
*/

#version 400 core

uniform sampler2D tex0;

in vec2 o_texCoords;

out vec4 fragColor;
	
void main() {
	fragColor = texture(tex0, o_texCoords);
}