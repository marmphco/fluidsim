/*
    mjframebuffer.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_FRAMEBUFFER_H
#define MJ_FRAMEBUFFER_H

#include "mjutil.h"
#include "mjtexture.h"
#include <vector>

namespace mcjee {

class Framebuffer {
private:
    std::vector<GLuint> renderbuffers;
	GLuint framebuffer;
    int _width;
    int _height;

public:
	Framebuffer(float width, float height);
	~Framebuffer();

    void addRenderTarget(GLenum format, GLenum attachment);
    void addRenderTarget(Texture2D *, GLenum attachment);

	void bind();
	void unbind();
};

}

#endif
