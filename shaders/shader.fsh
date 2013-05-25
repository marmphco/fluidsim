/*
    terrainShader.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
varying vec4 fTexCoord;
uniform sampler3D texture0;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

const int samples = 30;
const float fsamples = 30.0;

// only works on matrices with just rotate and translate
// isomorphic?
mat4 inverse(mat4 matrix) {
    mat4 rotation = mat4(1.0);
    rotation[0].xyz = matrix[0].xyz;
    rotation[1].xyz = matrix[1].xyz;
    rotation[2].xyz = matrix[2].xyz;
    mat4 inverseTranslate = mat4(1.0);
    inverseTranslate[3].xyz = -matrix[3].xyz;
    mat4 inverseRotation = transpose(rotation);
    return inverseRotation * inverseTranslate;
}

// oh yeah shitty ray marching
void main() {    
    mat4 inverseModelViewMatrix = transpose(inverse(viewMatrix * modelMatrix));

    vec4 eyeCoord = vec4(0, 0, 0, 1.0) * inverseModelViewMatrix;
    vec4 ray = normalize(fPosition-eyeCoord);
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    float tag = 1.0;
    for (int i = 0; i < samples; ++i) {
        float s = float(i)*1.73/fsamples; //should travel sqrt(2) distance
        vec3 pos = (fPosition+ray*s).xyz;
        //if (texture3D(texture0, pos).a != 0.0) {
        color = min(color+texture3D(texture0, pos)*0.05, 1.0);
        //tag = max(tag-color.a*99.0, 0.0);
        //}
        //tag = max(tag-color.a*9999.0, 0.0);
    }
    //color = texture3D(texture0, (fPosition).xyz*0.99);
    gl_FragColor = vec4(0.0, 0.00, 0.00, 0.1)+color; 
}
