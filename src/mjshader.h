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
        void setUniform1f(const char *name, float);
        void setUniform2f(const char *name, float, float);
        void setUniform3f(const char *name, float, float, float);
        void setUniform4f(const char *name, float, float, float, float);

        void setUniform1i(const char *name, int);
        void setUniform2i(const char *name, int, int);
        void setUniform3i(const char *name, int, int, int);
        void setUniform4i(const char *name, int, int, int, int);

        /*void setUniform1ui(const char *name, unsigned int);
        void setUniform2ui(const char *name, unsigned int, unsigned int);
        void setUniform3ui(const char *name, unsigned int, unsigned int, unsigned int);
        void setUniform4ui(const char *name, unsigned int, unsigned int, unsigned int, unsigned int);
        */
        //setUniformxv

        //setuniformmatrix

        void dumpUniforms();
        void dumpAttributes();
    };
}

#endif
