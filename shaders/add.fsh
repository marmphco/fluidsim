// advect.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
uniform sampler2D inBuffer;
uniform sampler2D velocityBuffer;

void main() {    
    vec4 stuff = texture2D(inBuffer, fPosition.xy);
    vec4 crap = texture2D(velocityBuffer, fPosition.xy);
    gl_FragData[0] = stuff+crap;
}
