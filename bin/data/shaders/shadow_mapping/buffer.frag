#version 400 core

out float fragDepth;

void main() {
	fragDepth = gl_FragCoord.z / gl_FragCoord.w;
}