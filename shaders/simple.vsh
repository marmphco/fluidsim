/*
    simple.vsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

attribute vec4 vPosition;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vPosition;
}
