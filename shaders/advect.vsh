/*
    terrainShader.vsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

attribute vec4 vPosition;
varying vec4 fPosition;

void main() {
    fPosition = vPosition*0.5+vec4(0.5, 0.5, 0.0, 0.0);
    gl_Position = vPosition;
}
