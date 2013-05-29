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

const int samples = 64;
const float fsamples = 64.0;
const float samplestep = 1.7320508/fsamples;

// oh yeah shitty ray marching
void main() {
    //transform to model space
    mat4 inverseModelViewMatrix = transpose(inverseModelMatrix * inverseViewMatrix);
    vec4 eyeCoord = vec4(0, 0, 0, 1.0) * inverseModelViewMatrix;
    vec4 ray = normalize(fPosition-eyeCoord);
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < samples; ++i) {     
        float s = float(i)*samplestep; //should travel sqrt(3) distance
        vec3 pos = (fPosition+ray*s).xyz;
        if (pos.x > 1.0 || pos.y > 1.0 || pos.z > 1.0) break; // outside the volume
        if (pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0) break; // outside the volume
        vec4 sample = texture3D(texture0, pos);
        float samplealpha = sample.a;//(sample.r+sample.g+sample.b)/3;
        //color.rgb = sample.rgb*samplealpha+color.rgb*(1.0-samplealpha);
        //color.a = sample.a*samplealpha+color.a*(1.0-samplealpha);
        color.rgb += sample.rgb*samplealpha*(1.0-color.a);
        color.a += samplealpha*(1.0-color.a);
    }
    gl_FragColor = min(color, vec4(1.0));
}
