/*
    rayPass.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fTexCoord;

uniform mat4 inverseViewMatrix;
uniform mat4 inverseModelMatrix;

void main() {
    mat4 inverseModelViewMatrix = transpose(inverseModelMatrix * inverseViewMatrix);
    vec4 eyeCoord = vec4(0, 0, 0, 1.0) * inverseModelViewMatrix;

    gl_FragColor = vec4(fTexCoord.xyz, distance(fTexCoord.xyz, eyeCoord.xyz));
}
