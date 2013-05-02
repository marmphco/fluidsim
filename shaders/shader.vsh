// terrainShader.vsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

attribute vec4 vPosition;
attribute vec4 vTexCoord;

varying vec4 fPosition;
varying vec4 fTexCoord;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main() {
    //fPosition = viewMatrix * modelMatrix * vPosition;
    fPosition = vPosition;
    fTexCoord = vTexCoord;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vPosition;
}
