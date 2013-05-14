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

        // Should also be catching IO exceptions
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

        getProgramUniforms();
        getProgramAttributes();

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

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0) {  
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            char log[length];
            glGetShaderInfoLog(shader, length, &status, log);
            string message("Shader Compile Failed: ");
            message += srcPath;
            message += "\n    ";
            message += log;

            glDeleteShader(shader);
            delete source;

            throw ShaderError(message);
        }

        return shader;
    }

    void Shader::getProgramUniforms() {
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
            // fix uniform array names
            if (name[nameSize-1] == ']') {
                name[nameSize-3] = '\0';
            } else {
                name[nameSize] = '\0'; // just in case
            }
            uniformMap[name] = glGetUniformLocation(program, name);
            std::cerr << name << ", " << uniformMap[name] << std::endl;
        }
    }

    void Shader::getProgramAttributes() {
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
            attributeMap[name] = glGetAttribLocation(program, name);
	    //std::cerr << name << ", " << attributeMap[name] << std::endl;
        }

    }

    void Shader::use() {
        glUseProgram(program);
    }

    // These are not safe right now for empty access
    GLint Shader::getAttribLocation(const GLchar *name) {
        return attributeMap[name];
    }

    GLint Shader::getUniformLocation(const GLchar *name) {
        return uniformMap[name];
    }

    bool Shader::uniformEnabled(const char *name) {
        return uniformMap.find(name) != uniformMap.end();
    }

    bool Shader::attributeEnabled(const char *name) {
        return attributeMap.find(name) != attributeMap.end();
    }
}
