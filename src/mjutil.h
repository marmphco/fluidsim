/*
    mjutil.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_UTIL_H
#define MJ_UTIL_H

#include <iostream>

#ifdef __APPLE__
/*#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-W#warnings"
#       include <OpenGL/gl3.h>
#   pragma clang diagnostic pop*/
    #   include "libglew/glew.h"
#   include <GLUT/glut.h>
#   undef glGenVertexArrays
#   undef glBindVertexArray
#   undef glDeleteVertexArrays
#   define glGenVertexArrays glGenVertexArraysAPPLE
#   define glBindVertexArray glBindVertexArrayAPPLE
#   define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#else
#   include "libglew/glew.h"
#   include <GL/glut.h>
#endif

#include "mjmath.h"

long getTimeMillis(void);

#endif
