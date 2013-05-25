/*
    divergence.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
uniform sampler2D velocityBuffer;

uniform int iterations;
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
    vec3 p = to3Space(fPosition.xy);
    vec3 xu = vec3(xunit, 0.0, 0.0);
    vec3 yu = vec3(0.0, xunit, 0.0);
    vec3 zu = vec3(0.0, 0.0, xunit);

    float dx = texture2D(velocityBuffer, to2SpaceZFloor(p+xu)).x-texture2D(velocityBuffer, to2SpaceZFloor(p-xu)).x;
    float dy = texture2D(velocityBuffer, to2SpaceZFloor(p+yu)).y-texture2D(velocityBuffer, to2SpaceZFloor(p-yu)).y;
    float dz = texture2D(velocityBuffer, to2SpaceZFloor(p+zu)).z-texture2D(velocityBuffer, to2SpaceZFloor(p-zu)).z;
    gl_FragData[0] = vec4(0.5*dx+dy+dz);
}
