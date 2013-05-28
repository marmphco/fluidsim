/*
    shader.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
varying vec4 fTexCoord;
uniform sampler3D texture0;

uniform mat4 inverseViewMatrix;
uniform mat4 inverseModelMatrix;

const int samples = 40;
const float fsamples = 40.0;

// oh yeah shitty ray marching
void main() {
    //transform to model space
    mat4 inverseModelViewMatrix = transpose(inverseModelMatrix * inverseViewMatrix);
    vec4 eyeCoord = vec4(0, 0, 0, 1.0) * inverseModelViewMatrix;
    vec4 ray = normalize(fPosition-eyeCoord);
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < samples; ++i) {
        float s = float(i)*1.73/fsamples; //should travel sqrt(3) distance
        vec3 pos = (fPosition+ray*s).xyz;
        if (pos.x > 1.0 || pos.y > 1.0 || pos.z > 1.0) break; // outside the volume
        color = min(color+texture3D(texture0, pos)*0.1, 1.0);
    }
    gl_FragColor = vec4(0.0, 0.00, 0.00, 0.1)+color; 
}
