/*
    mjshader.h
    Matthew Jee
    mcjee@ucsc.edu

    A wrapper around OpenGL shader programs.
 */

#ifndef MJ_SHADER_H
#define MJ_SHADER_H

#include "mjutil.h"
#include <map>
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

        std::map<string, GLint> uniformMap;
        std::map<string, GLint> attributeMap;

        void readSourceFile(const char *srcPath, char **srcBuffer);
        GLuint compileShader(GLenum type, const GLchar *srcPath);
        void getProgramUniforms();
        void getProgramAttributes();
    public:
        GLuint program;
        Shader();
        ~Shader();
        void compile(const char *vSrcPath, const char *fSrcPath);
        GLint getAttribLocation(const GLchar *name);
        GLint getUniformLocation(const GLchar *name);
        void use(void);

        bool uniformEnabled(const char *name);
        bool attributeEnabled(const char *name);
    };
}

#endif
