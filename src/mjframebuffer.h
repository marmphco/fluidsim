/*
    mjframebuffer.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_FRAMEBUFFER_H
#define MJ_FRAMEBUFFER_H

#include "mjutil.h"
#include "mjshader.h"

namespace mcjee {

class Framebuffer {
private:
	GLuint framebuffer;
    GLuint colorbuffer;
    GLuint depthbuffer;
    GLuint pickbuffer;

public:
	Framebuffer(float width, float height);
	~Framebuffer();

	void bind();
	void unbind();
	void present();
};

}

#endif
