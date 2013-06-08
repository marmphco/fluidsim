/*
    mjshader.c
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjshader.h"

#include <cstdio>
#include <fstream>

namespace mcjee {
    ShaderError::ShaderError(const string &message) :
        runtime_error(message) {}

    Shader::Shader() : linked(false) {}

    Shader::~Shader() {
        if (linked) glDeleteProgram(program);
    }

    void Shader::readSourceFile(const char *srcPath, char **srcBuffer) {
        ifstream fs(srcPath, ifstream::in);

        fs.seekg(0, fs.end);
        int length = fs.tellg();
        fs.seekg(0, fs.beg);

        *srcBuffer = new char[length+1];

        fs.read(*srcBuffer, length);
        (*srcBuffer)[length] = '\0';

        fs.close();
    }

    void Shader::compile(const char *vSrcPath, const char *fSrcPath) {

        program = glCreateProgram();
        GLuint vertexShader, fragShader;
        vertexShader = compileShader(GL_VERTEX_SHADER, vSrcPath);
        fragShader = compileShader(GL_FRAGMENT_SHADER, fSrcPath);
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == 0) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragShader);
            glDeleteProgram(program);

            string message("Shader Link Failed: ");
            message += vSrcPath;
            message += ", ";
            message += fSrcPath;
            throw ShaderError(message);
        }

        linked = true;
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    GLuint Shader::compileShader(GLenum type, const char *srcPath) {
        GLchar *source;
        readSourceFile(srcPath, &source);

        const GLchar *csource = source;
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &csource, NULL);
        glCompileShader(shader);
        delete source;

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0) {  
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            char *log = new char[length];
            glGetShaderInfoLog(shader, length, &status, log);
            string message("Shader Compile Failed: ");
            message += srcPath;
            message += "\n    ";
            message += log;

            glDeleteShader(shader);
            delete log;
            throw ShaderError(message);
        }

        return shader;
    }

    void Shader::dumpUniforms() {
        GLint uniformCount;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);
        for (int i = 0; i < uniformCount; ++i) {
            GLchar name[64];
            GLsizei nameSize;
            GLint size;
            GLenum type;
            glGetActiveUniform(program,
                               i,
                               sizeof(name)-1,
                               &nameSize,
                               &size,
                               &type,
                               name);
            name[nameSize] = '\0'; // just in case
            GLint loc = glGetUniformLocation(program, name);
            std::cerr << name << ", " << loc << std::endl;
        }
    }

    void Shader::dumpAttributes() {
        GLint attribCount;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribCount);
        for (int i = 0; i < attribCount; ++i) {
            GLchar name[64];
            GLsizei nameSize;
            GLint size;
            GLenum type;
            glGetActiveAttrib(program,
                              i,
                              sizeof(name)-1,
                              &nameSize,
                              &size,
                              &type,
                              name);
            name[nameSize] = '\0'; // just in case
            GLint loc = glGetAttribLocation(program, name);
	        std::cerr << name << ", " << loc << std::endl;
        }

    }

    void Shader::use() {
        glUseProgram(program);
    }

    GLint Shader::getAttribLocation(const GLchar *name) {
        return glGetAttribLocation(program, name);
    }

    GLint Shader::getUniformLocation(const GLchar *name) {
        return glGetUniformLocation(program, name);
    }

    bool Shader::uniformEnabled(const char *name) {
        return glGetUniformLocation(program, name) != -1;
    }

    bool Shader::attributeEnabled(const char *name) {
        return glGetAttribLocation(program, name) != -1;
    }

//to help define all those uniform setting functions
#define uniform1x(POSTFIX, TYPE) void Shader::setUniform1##POSTFIX(const char *name, TYPE x) {\
    GLint loc = getUniformLocation(name);\
    if (loc != -1) glUniform1##POSTFIX(loc, x);\
}

#define uniform2x(POSTFIX, TYPE) void Shader::setUniform2##POSTFIX(const char *name, TYPE x, TYPE y) {\
    GLint loc = getUniformLocation(name);\
    if (loc != -1) glUniform2##POSTFIX(loc, x, y);\
}

#define uniform3x(POSTFIX, TYPE) void Shader::setUniform3##POSTFIX(const char *name, TYPE x, TYPE y, TYPE z) {\
    GLint loc = getUniformLocation(name);\
    if (loc != -1) glUniform3##POSTFIX(loc, x, y, z);\
}

#define uniform4x(POSTFIX, TYPE) void Shader::setUniform4##POSTFIX(const char *name, TYPE x, TYPE y, TYPE z, TYPE w) {\
    GLint loc = getUniformLocation(name);\
    if (loc != -1) glUniform4##POSTFIX(loc, x, y, z, w);\
}

    uniform1x(f, float)
    uniform2x(f, float)
    uniform3x(f, float)
    uniform4x(f, float)
    uniform1x(i, int)
    uniform2x(i, int)
    uniform3x(i, int)
    uniform4x(i, int)
    /*uniform1x(ui, unsigned int)
    uniform2x(ui, unsigned int)
    uniform3x(ui, unsigned int)
    uniform4x(ui, unsigned int)*/


}
