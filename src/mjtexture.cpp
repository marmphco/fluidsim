/*
    mjtexture.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjtexture.h"

namespace mcjee {

Texture::Texture(GLenum target, GLint internalFormat, GLenum format, GLenum type) :
    target(target), internalFormat(internalFormat), format(format), type(type) {
    glGenTextures(1, &texture);
    bind();
    // non-sensical defaults, temporary
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void Texture::bind(void) {
    glBindTexture(target, texture);
}

Texture2D::Texture2D(GLint internalFormat, GLenum format, GLenum type,
                     int width, int height) :
    Texture(GL_TEXTURE_2D, internalFormat, format, type),
    width(width), height(height) {

}

Texture3D::Texture3D(GLint internalFormat, GLenum format, GLenum type,
                     int width, int height, int depth) :
    Texture(GL_TEXTURE_3D, internalFormat, format, type),
    width(width), height(height), depth(depth) {

}

}