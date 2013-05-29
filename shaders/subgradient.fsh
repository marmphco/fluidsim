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

void main() {
    float b = 1.0; // scaling term that should be uniformed

    vec2 left = vec2(fPosition.x-xunit, fPosition.y);
    vec2 right = vec2(fPosition.x+xunit, fPosition.y);
    vec2 up = vec2(fPosition.x, fPosition.y+yunit);
    vec2 down = vec2(fPosition.x, fPosition.y-yunit);
    vec2 forward = vec2(fPosition.x, fPosition.y+sliceHeight);
    vec2 backward = vec2(fPosition.x, fPosition.y-sliceHeight);

    float dx = (texture2D(scalarBuffer, right).x-texture2D(scalarBuffer, left).x)*0.5/b;
    float dy = (texture2D(scalarBuffer, up).x-texture2D(scalarBuffer, down).x)*0.5/b;
    float dz = (texture2D(scalarBuffer, forward).x-texture2D(scalarBuffer, backward).x)*0.5/b;
    gl_FragData[0] = texture2D(velocityBuffer, fPosition.xy)-vec4(dx, dy, dz, 0.0);
}
