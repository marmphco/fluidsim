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

void main() {
    vec2 left = vec2(fPosition.x-xunit, fPosition.y);
    vec2 right = vec2(fPosition.x+xunit, fPosition.y);
    vec2 up = vec2(fPosition.x, fPosition.y+yunit);
    vec2 down = vec2(fPosition.x, fPosition.y-yunit);
    vec2 forward = vec2(fPosition.x, fPosition.y+sliceHeight);
    vec2 backward = vec2(fPosition.x, fPosition.y-sliceHeight);

    float dx = texture2D(velocityBuffer, right).x-texture2D(velocityBuffer, left).x;
    float dy = texture2D(velocityBuffer, up).y-texture2D(velocityBuffer, down).y;
    float dz = texture2D(velocityBuffer, forward).z-texture2D(velocityBuffer, backward).z;
    gl_FragData[0] = vec4(0.5*dx+dy+dz);
}
