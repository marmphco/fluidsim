/*
    divergence.fsh
    Matthew Jee
    mcjee@ucsc.edu
*/

#version 120

varying vec4 fPosition;
uniform sampler2D divergenceBuffer;
uniform sampler2D pressureBuffer;

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

    float dx = texture2D(pressureBuffer, right).x+texture2D(pressureBuffer, left).x;
    float dy = texture2D(pressureBuffer, up).x+texture2D(pressureBuffer, down).x;
    float dz = texture2D(pressureBuffer, forward).x+texture2D(pressureBuffer, backward).x;
    float divergence = texture2D(divergenceBuffer, fPosition.xy).r;
    gl_FragData[0] = vec4((dx+dy+dz-divergence)/6);
}
