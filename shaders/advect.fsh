// advect.fsh
// Matthew Jee
// mcjee@ucsc.edu
#version 120

varying vec4 fPosition;
uniform sampler2D inBuffer;
uniform sampler2D velocityBuffer;

// make these uniforms
// also add dt uniform
const int width = 32;
const int height = 32;
const int depth = 32;
const float xunit = 1.0/32.0;
const float yunit = 1.0/(32.0*32.0);
const float h = height*yunit;

vec3 to3Space(vec2 vec) {
    return vec3(vec.x, mod(vec.y, h)*32.0, floor(vec.y/h)/32.0);
}

vec2 to2SpaceZFloor(vec3 vec) {
    return vec2(vec.x, vec.y*h+h*floor(vec.z*32.0));
}

vec2 to2SpaceZCeil(vec3 vec) {
    return vec2(vec.x, vec.y*h+h*ceil(vec.z*32.0));
}

//needs lerp across z axis
void main() {
    vec3 origin = to3Space(fPosition.xy); 
    vec4 stuff = texture2D(velocityBuffer, fPosition.xy);
    vec4 crap = texture2D(inBuffer, fPosition.xy);

    vec3 velocity = texture2D(velocityBuffer, fPosition.xy).xyz;
    vec3 source = origin-velocity*0.03;

    //shitty z-lerp
    vec4 col1 = texture2D(inBuffer, to2SpaceZFloor(source));
    vec4 col2 = texture2D(inBuffer, to2SpaceZCeil(source));

     gl_FragData[0] = (col1+col2)*0.5;
}
