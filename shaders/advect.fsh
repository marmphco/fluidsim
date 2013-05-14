// advect.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
uniform sampler2D inBuffer;
uniform sampler2D velocityBuffer;

void main() {    
    vec2 texSpace = fPosition.xy;
    vec4 stuff = texture2D(velocityBuffer, texSpace);
    gl_FragColor = texture2D(inBuffer, texSpace); 
}
