/*
    colorBlend.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fTexCoord;
//uniform sampler2D rayBuffer;
uniform sampler3D texture0;

uniform mat4 inverseViewMatrix;
uniform mat4 inverseModelMatrix;

const int samples = 64;
const float fsamples = 64.0;
const float samplestep = 1.7320508/fsamples;

const vec3 redChannel = vec3(1.0, 0.0, 0.0);
const vec3 greenChannel = vec3(0.0, 1.0, 0.0);
const vec3 blueChannel = vec3(0.0, 0.0, 1.0);
const vec3 alphaChannel = vec3(1.0, 1.0, 1.0);

void main() {
    //transform to texture space
    mat4 inverseModelViewMatrix = transpose(inverseModelMatrix * inverseViewMatrix);
    vec4 eyeCoord = vec4(0, 0, 0, 1.0) * inverseModelViewMatrix;
    vec4 ray = normalize(fTexCoord-eyeCoord);
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 lightDir = normalize((inverseModelViewMatrix * vec4(0.0, 1.0, 0.0, 1.0)).xyz);
    for (int i = 0; i < samples; ++i) {     
        float s = float(i)*samplestep; //should travel sqrt(3) distance
        vec3 pos = (fTexCoord+ray*s).xyz;
        if (pos.x > 1.0 || pos.y > 1.0 || pos.z > 1.0) break; // outside the volume
        if (pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0) break; // outside the volume
        vec4 sample = texture3D(texture0, pos);

        float alpha = sample.r+sample.g+sample.b+sample.a;
        sample.rgb = (redChannel*sample.r+greenChannel*sample.g+blueChannel*sample.b+alphaChannel*sample.a)/alpha;

        color.rgb += sample.rgb*alpha*(1.0-color.a);
        color.a += alpha*(1.0-color.a);

        if (color.a > 1.0) {
            color.a = 1.0;
            break;
        }
    }
    //vec4 shit = texture2D(rayBuffer, vec2(0.0, 0.0));

    gl_FragColor = min(color, vec4(1.0));
}
