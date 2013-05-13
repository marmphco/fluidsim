/*
    mjutil.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_UTIL_H
#define MJ_UTIL_H

#include <iostream>

#ifdef __APPLE__
#   include <GLUT/glut.h>
#   define glGenVertexArrays glGenVertexArraysAPPLE
#   define glBindVertexArray glBindVertexArrayAPPLE
#   define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#else
#   include "libglew/glew.h"
#   include <GL/glut.h>
#endif

long getTimeMillis(void);

#endif
