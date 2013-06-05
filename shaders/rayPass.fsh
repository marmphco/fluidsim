/*
    rayPass.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
varying vec4 fTexCoord;

uniform mat4 inverseViewMatrix;
uniform mat4 inverseModelMatrix;

void main() {
    gl_FragColor = vec4(fPosition.xyz, length(fPosition));
}
