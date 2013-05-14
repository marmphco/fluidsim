// terrainShader.vsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

attribute vec4 vPosition;
attribute vec4 vTexCoord;

varying vec4 fPosition;
varying vec4 fTexCoord;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vPosition;
}
