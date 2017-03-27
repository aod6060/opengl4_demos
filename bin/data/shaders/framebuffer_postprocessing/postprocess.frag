/**
	basic.frag
	
	This is the 
*/

#version 400 core

uniform sampler2D tex0;

uniform sampler2D depth0;

uniform vec2 mouseVel;

in vec2 o_texCoords;

out vec4 fragColor;

// Greyscale
vec4 greyscale() {

	vec3 color = texture(tex0, o_texCoords).rgb;
	
	vec3 average = vec3((color.r + color.g + color.b)/3.0);
	
	return vec4(average, 1.0);
}

vec4 sepai_tone() {
	vec3 color = texture(tex0, o_texCoords).rgb;
	
	vec3 outputColor = color;
	
	mat3 sepai = mat3(
		vec3(0.393, 0.769, 0.189),
		vec3(0.349, 0.686, 0.168),
		vec3(0.272, 0.534, 0.131)
	);
	
	outputColor = transpose(sepai) * color;
	
	return vec4(outputColor, 1.0);
}

vec4 inverse_color() {
	vec3 color = texture(tex0, o_texCoords).rgb;
	
	vec3 outputColor = 1.0 - color;
	
	return vec4(outputColor, 1.0);
}

vec4 enhance_greyscale() {
	vec3 color = texture(tex0, o_texCoords).rgb;
	
	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	
	return vec4(average, average, average, 1.0);
}

#define GAUSSIAN_BLUR_SIZE 5
vec4 gaussian_blur() {
	float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
	float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
	
	vec3 color = texture(tex0, vec2(gl_FragCoord)/1366.0).rgb * weight[0];
	
	for(int i = 1; i < GAUSSIAN_BLUR_SIZE; i++) {
		color += texture(tex0, (vec2(gl_FragCoord)+vec2(0.0, offset[i])) / 1366.0).rgb * weight[i];
		color += texture(tex0, (vec2(gl_FragCoord)-vec2(0.0, offset[i])) / 1366.0).rgb * weight[i];
	}
	
	return vec4(color, 1.0);
}

vec4 blur1(vec2 offset) {
	
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
		sampleTex[i] = vec3(texture(tex0, o_texCoords + offsets[i]));
	}
	
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++) {
		col += sampleTex[i] * kernel[i];
	}
	
	return vec4(col, 1.0);
}

vec4 motion_blur() {
	return blur1(mouseVel / 64.0);
}

vec4 dof_blur() {
	//vec4 depthMap = texture(depth0, o_texCoords);
	//return blur1(vec2(1.0 / 256 * blur));
	
	float d = gl_FragCoord.z / gl_FragCoord.w;
	
	float sampleOffset = d / 300.0;
	
	
	return vec4(vec3(d), 1.0);
}

vec4 bloom() {
	vec3 color = texture(tex0, o_texCoords).xyz;
	
	
	//return vec4((color + blur) * 0.5, 1.0);
	float offset = 0.9;
	
	if(color.r > offset || color.g > offset || color.b > offset) {
		return vec4((color * blur1(vec2(1.0 / 64.0)).xyz), 1.0);
	}
	
	return vec4(color, 1.0);
}

void main() {
	fragColor = bloom();
}