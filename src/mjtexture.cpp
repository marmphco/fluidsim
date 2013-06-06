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
    // non-sensical defaults
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    unbind();
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void Texture::bind(void) {
    glBindTexture(target, texture);
}

void Texture::unbind(void) {
    glBindTexture(target, 0);
}

void Texture::interpolation(GLint interpol) {
    bind();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, interpol);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, interpol);
    unbind();
}

void Texture::wrap(GLint wrap) {
    bind();
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
    unbind();
}

Texture2D::Texture2D(GLint internalFormat, GLenum format, GLenum type,
                     int width, int height) :
    Texture(GL_TEXTURE_2D, internalFormat, format, type),
    width(width), height(height) {

}

void Texture2D::present(Shader *shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //GL_ONE, GL_ZERO
    GLfloat vertices[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
    };
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint loc = shader->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture3D::Texture3D(GLint internalFormat, GLenum format, GLenum type,
                     int width, int height, int depth) :
    Texture(GL_TEXTURE_3D, internalFormat, format, type),
    width(width), height(height), depth(depth) {

}

}
