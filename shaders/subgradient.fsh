/*
    divergence.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
uniform sampler2D scalarBuffer;
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
    float b = 1.0; // scaling term that should be uniformed

    vec3 p = to3Space(fPosition.xy);
    vec3 xu = vec3(xunit, 0.0, 0.0);
    vec3 yu = vec3(0.0, xunit, 0.0);
    vec3 zu = vec3(0.0, 0.0, xunit);

    float dx = (texture2D(scalarBuffer, to2SpaceZFloor(p+xu)).x-texture2D(scalarBuffer, to2SpaceZFloor(p-xu)).x)*0.5/b;
    float dy = (texture2D(scalarBuffer, to2SpaceZFloor(p+yu)).x-texture2D(scalarBuffer, to2SpaceZFloor(p-yu)).x)*0.5/b;
    float dz = (texture2D(scalarBuffer, to2SpaceZFloor(p+zu)).x-texture2D(scalarBuffer, to2SpaceZFloor(p-zu)).x)*0.5/b;
    gl_FragData[0] = texture2D(velocityBuffer, fPosition.xy)-vec4(dx, dy, dz, 0.0);
}
