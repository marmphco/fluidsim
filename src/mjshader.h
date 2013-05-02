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

        bool _ambientEnabled;
        bool _lightEnabled;
        bool _diffuseEnabled;
        bool _specularEnabled;
        bool _projectionMatrixEnabled;
        bool _viewMatrixEnabled;
        bool _modelMatrixEnabled;

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
        void compileProgram(const char *vSrcPath, const char *fSrcPath);
        GLint getAttribLocation(const GLchar *name);
        GLint getUniformLocation(const GLchar *name);
        void use(void);

        bool ambientEnabled() {return _ambientEnabled;};
        bool lightEnabled() {return _lightEnabled;};
        bool diffuseEnabled() {return _diffuseEnabled;};
        bool specularEnabled() {return _specularEnabled;};
        bool projectionMatrixEnabled() {return _projectionMatrixEnabled;};
        bool viewMatrixEnabled() {return _viewMatrixEnabled;};
        bool modelMatrixEnabled() {return _modelMatrixEnabled;};
    };
}

#endif
