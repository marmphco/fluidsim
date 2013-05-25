// advect.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
uniform sampler2D buffer0;
uniform sampler2D buffer1;

void main() {    
    vec4 stuff = texture2D(buffer0, fPosition.xy);
    vec4 crap = texture2D(buffer1, fPosition.xy);
    gl_FragData[0] = stuff+crap;
}
