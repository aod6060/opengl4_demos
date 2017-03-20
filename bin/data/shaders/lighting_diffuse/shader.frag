/**
	shader.frag
	
	This is the fragment shader for our simple triangle...
*/

#version 400 core

uniform sampler2D tex0;

in vec2 o_texCoords;

in vec3 o_color;

out vec4 fragColor;

void main() {
	fragColor = texture(tex0, o_texCoords) * vec4(o_color, 1.0);
	//fragColor = vec4(o_texCoords, 0.0, 1.0);
}
