/**
	basic.frag
	
	This is the 
*/

#version 400 core

uniform sampler2D tex0;

uniform sampler2D color0;

uniform vec2 mouseVel;

uniform float exposure;

in vec2 o_texCoords;

out vec4 fragColor;

vec4 blur(vec2 offset) {
	
	vec2 offsets[9] = vec2[] (
		vec2(-offset.x, offset.y),
		vec2(0.0, offset),
		vec2(offset.x, offset.y),
		vec2(-offset.x, 0.0),
		vec2(0.0, 0.0),
		vec2(offset.x, 0.0),
		vec2(-offset.x, -offset.y),
		vec2(0.0f, -offset.x),
		vec2(offset.x, -offset.y)
	);
	
	float kernel[9] = float[] (
		0.0625, 0.1250, 0.0625,
		0.1250, 0.2500, 0.1250,
		0.0625, 0.1250, 0.0625
	);
	
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(color0, o_texCoords + offsets[i]));
	}
	
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++) {
		col += sampleTex[i] * kernel[i];
	}
	
	return vec4(col, 1.0);
}

void main() {
	const float gamma = 2.2;
	vec3 color = texture(tex0, o_texCoords).rgb;
	vec3 blur = blur(vec2(1.0 / 300.0)).rgb;
	
	color += blur;
	
	vec3 result = vec3(1.0) - exp(-color * exposure);
	//result = pow(result, vec3(1.0 / gamma));
	fragColor = vec4(result, 1.0);
	//fragColor = vec4(color + blur, 1.0);
}