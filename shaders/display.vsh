/*
    display.vsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
attribute vec4 vPosition;

uniform float aspectRatio; //cuts off and letterboxes x axis

void main() {
	//fPosition = vPosition*0.5+vec4(0.5, 0.5, 0.0, 0.0);
    fPosition.x = vPosition.x*0.5*aspectRatio+0.5;
    fPosition.y = vPosition.y*0.5+0.5;
    gl_Position = vPosition;
}
