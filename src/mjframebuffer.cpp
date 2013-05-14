/*
    mjframebuffer.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjframebuffer.h"

namespace mcjee {


Framebuffer::Framebuffer(float width, float height) {
	glGenFramebuffers(1, &framebuffer);
    _width = width;
    _height = height;

}

Framebuffer::~Framebuffer() {
	//glDeleteRenderbuffers(1, &colorbuffer);
	//glDeleteRenderbuffers(1, &depthbuffer);
	//glDeleteRenderbuffers(1, &pickbuffer);
	//glDeleteFramebuffers(1, &framebuffer);
    //delete everything in renderbuffers?
}

void Framebuffer::addRenderTarget(GLenum format, GLenum attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLuint buffer;
    glGenRenderbuffers(1, &buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, format, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer);
    renderbuffers.push_back(buffer);
}

void Framebuffer::addRenderTarget(Texture2D *texture, GLenum attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    texture->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->texture, 0);
    renderbuffers.push_back(texture->texture);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
