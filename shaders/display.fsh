// terrainShader.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
uniform sampler2D texture0;

void main() {
    gl_FragColor = texture2D(texture0, fPosition.xy*0.5+vec2(0.5, 0.5));
}
