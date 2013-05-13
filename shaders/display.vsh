// terrainShader.vsh
// Matthew Jee
// mcjee@ucsc.edu

#version 120

varying vec4 fPosition;
attribute vec4 vPosition;

void main() {
	fPosition = vPosition;
    gl_Position = vPosition;
}
