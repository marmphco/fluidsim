/*
    mjshader.h
    Matthew Jee
    mcjee@ucsc.edu

    A wrapper around OpenGL shader programs.
 */

#ifndef MJ_SHADER_H
#define MJ_SHADER_H

#include "mjutil.h"
#include <string>
#include <stdexcept>
#include <exception>

using namespace std;

namespace mcjee {

class ShaderError : public runtime_error {
public:
    ShaderError(const string &message);
};

class Shader {
private:
    bool linked;

    void readSourceFile(const char *srcPath, char **srcBuffer);
    GLuint compileShader(GLenum type, const GLchar *srcPath);
public:
    GLuint program;
    Shader();
    ~Shader();
    void compile(const char *vSrcPath, const char *fSrcPath);
    void use(void);

    GLint getAttribLocation(const GLchar *name);
    GLint getUniformLocation(const GLchar *name);
    bool uniformEnabled(const char *name);
    bool attributeEnabled(const char *name);

    // The advantage these hold over just plain
    // glUniformX is that these check first if the name
    // exists, then set if it is safe to do so.
    // Also, it's convinient to write:
    //     shader->setUniformX(name, value)
    // rather than:
    //     GLint loc = glGetUniformLocation
    //     if (loc != -1) glUniformX
#define _uniform1x(POSTFIX, TYPE) void setUniform1##POSTFIX(const char *, TYPE);
#define _uniform2x(POSTFIX, TYPE) void setUniform2##POSTFIX(const char *, TYPE, TYPE);
#define _uniform3x(POSTFIX, TYPE) void setUniform3##POSTFIX(const char *, TYPE, TYPE, TYPE);
#define _uniform4x(POSTFIX, TYPE) void setUniform4##POSTFIX(const char *, TYPE, TYPE, TYPE, TYPE);
#define _uniformxv(POSTFIX, TYPE) void setUniform##POSTFIX##v(const char *, GLsizei, TYPE);
#define _uniformMatxv(POSTFIX) void setUniformMatrix##POSTFIX##v(const char *, GLsizei, GLboolean, const GLfloat *);

    _uniform1x(f, GLfloat)
    _uniform2x(f, GLfloat)
    _uniform3x(f, GLfloat)
    _uniform4x(f, GLfloat)
    _uniform1x(i, GLint)
    _uniform2x(i, GLint)
    _uniform3x(i, GLint)
    _uniform4x(i, GLint)
    _uniformxv(1f, const GLfloat *)
    _uniformxv(2f, const GLfloat *)
    _uniformxv(3f, const GLfloat *)
    _uniformxv(4f, const GLfloat *)
    _uniformxv(1i, const GLint *)
    _uniformxv(2i, const GLint *)
    _uniformxv(3i, const GLint *)
    _uniformxv(4i, const GLint *)
    _uniformMatxv(2f)
    _uniformMatxv(3f)
    _uniformMatxv(4f)
    _uniformMatxv(2x3f)
    _uniformMatxv(3x2f)
    _uniformMatxv(2x4f)
    _uniformMatxv(4x2f)
    _uniformMatxv(3x4f)
    _uniformMatxv(4x3f)
    //the version of opengl i'm compiling against 
    //does not have unsigned integer support

    void dumpUniforms();
    void dumpAttributes();
};

}

#endif
