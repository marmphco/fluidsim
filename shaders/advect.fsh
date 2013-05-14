// advect.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
varying vec4 fTexCoord;
uniform sampler3D texture0;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {    
    
    gl_FragColor = vec4(0.0, 0.00, 0.00, 0.1)+color; 
}
