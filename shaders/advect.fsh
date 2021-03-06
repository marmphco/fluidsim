/*
    advect.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
uniform sampler2D inBuffer;
uniform sampler2D velocityBuffer;

uniform float dt;
uniform float width;
uniform float height;
uniform float depth;
uniform float xunit;
uniform float yunit;
uniform float sliceHeight;

vec3 to3Space(vec2 vec) {
    return vec3(vec.x, mod(vec.y, sliceHeight)/sliceHeight, floor(vec.y/sliceHeight)/depth);
}

vec2 to2SpaceZFloor(vec3 vec) {
    return vec2(vec.x, vec.y*sliceHeight+sliceHeight*floor(vec.z*depth));
}

vec2 to2SpaceZCeil(vec3 vec) {
    return vec2(vec.x, vec.y*sliceHeight+sliceHeight*ceil(vec.z*depth));
}

void main() {
    //shitty fix to the y-bleeding problem
    if (mod(fPosition.y, sliceHeight) < sliceHeight/height) {
        discard;
    }
    
    vec3 origin = to3Space(fPosition.xy);
    vec4 stuff = texture2D(velocityBuffer, fPosition.xy);

    vec3 velocity = texture2D(velocityBuffer, fPosition.xy).xyz;
    vec3 source = origin-velocity*dt;

    //z-lerp
    vec2 p1 = to2SpaceZFloor(source);
    vec2 p2 = to2SpaceZCeil(source);
    vec4 col1 = texture2D(inBuffer, p1);
    vec4 col2 = texture2D(inBuffer, p2);
    float f = 1-mod(source.z, xunit)/xunit; //these are actually zunit?

    gl_FragData[0] = col1*f+col2*(1-f);
}
