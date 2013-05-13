/*
    mjframebuffer.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjframebuffer.h"

namespace mcjee {

static Shader *displayShader;

Framebuffer::Framebuffer(float width, float height) {
	glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &pickbuffer);
    glGenRenderbuffers(1, &depthbuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, pickbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, pickbuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenTextures(1, &colorbuffer);
    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);      

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer incomplete" << std::endl;
    }
    if (displayShader == NULL) {
    	displayShader = new Shader();
	    displayShader->compile("shaders/display.vsh", "shaders/display.fsh");
	}
}

Framebuffer::~Framebuffer() {
	glDeleteRenderbuffers(1, &colorbuffer);
	glDeleteRenderbuffers(1, &depthbuffer);
	glDeleteRenderbuffers(1, &pickbuffer);
	glDeleteFramebuffers(1, &framebuffer);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::present() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat vertices[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
    };
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    displayShader->use();
    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    GLint loc = displayShader->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

}
