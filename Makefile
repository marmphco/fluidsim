#
# The Simple OpenGL Makefile v0.00001
# Matthew Jee
# mcjee@ucsc.edu
# Should build on OS X.
# Tested with 10.8/x86_64
#

UNAME = ${shell uname}
ARCH = ${shell uname -m}
INCLUDEDEPS = ${filter clean,${MAKECMDGOALS}}

SUBMITNAME = finalProject 
BUNDLENAME = FluidSim
EXECNAME = fluidSim
SRCDIR = src
SHADERDIR = shaders
OBJDIR = obj
BINDIR = bin

_CSRC = final.cpp mjterrain.cpp mjshader.cpp mjrenderable.cpp\
        mjutil.cpp mjmath.cpp mjscene.cpp mjcamera.cpp mjlight.cpp\
        mjprimitive.cpp mjloader.cpp

_SHADERS = shader.fsh shader.vsh

EXECUTABLE = ${BINDIR}/${EXECNAME}
BUNDLE = ${BINDIR}/${BUNDLENAME}.app

CSRC = ${patsubst %,${SRCDIR}/%,${_CSRC}}
OBJ = ${patsubst %.cpp,${OBJDIR}/%.o,${_CSRC}}
SHADERS = ${patsubst %,${SHADERDIR}/%,${_SHADERS}}

OPTIONS = -Wall -Wextra
ifeq "${UNAME}" "Darwin"
# OS X/Darwin, using clang and stuff.
	COMPILER = clang++
	COMPILE = ${COMPILER} ${OPTIONS}
	FRAMEWORKS = -framework OpenGL -framework GLUT
	LINK = ${COMPILER} ${OPTIONS} ${FRAMEWORKS}
	MAINTARGET = bundle
	LIBS = ${SRCDIR}/libglui/libglui${UNAME}${ARCH}.a
endif

ifeq "${UNAME}" "Linux"
# Generic Linux
	COMPILER = g++
	COMPILE = ${COMPILER} ${OPTIONS}
	OTHERLIB = -lGL -lglut -lGLU
	LINK = ${COMPILER} ${OPTIONS} ${OTHERLIB}
	MAINTARGET = ${EXECUTABLE}
	LIBS = ${SRCDIR}/libglew/libglew${UNAME}${ARCH}.a\
	       ${SRCDIR}/libglui/libglui${UNAME}${ARCH}.a
endif

ifeq "${COMPILE}" ""
	${error echo System: ${UNAME} not supported.}
endif

all: ${MAINTARGET}

run: ${EXECUTABLE}
	./${EXECUTABLE}

submission: ${MAINTARGET}
	- mkdir ${SUBMITNAME} ${SUBMITNAME}/obj
	cp Makefile README.txt ${SUBMITNAME}
	cp -r shaders ${SUBMITNAME}
	cp -r src ${SUBMITNAME}
	cp -r bin ${SUBMITNAME}
	cp -r shaders ${SUBMITNAME}/bin
	cp index.html thumbnail.png ${SUBMITNAME}
	tar -cf ${SUBMITNAME}.tar ${SUBMITNAME}
	rm -rf ${SUBMITNAME}

bundle: ${BUNDLE}

${BUNDLE}: ${EXECUTABLE}
	mkdir -p ${BUNDLE}/Contents/MacOS
	mkdir -p ${BUNDLE}/Contents/Resources/shaders
	cp ${SHADERS} ${BUNDLE}/Contents/Resources/shaders
	cp ${EXECUTABLE} ${BUNDLE}/Contents/MacOS/${BUNDLENAME}

${EXECUTABLE}: ${OBJ} ${LIBS}
	${LINK} -o ${EXECUTABLE} ${OBJ} ${LIBS}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	${COMPILE} -c $< -o $@

deps: ${CSRC}
	${COMPILER} -MM ${CSRC} > deps

src/libglew/libglew%:
	cd src/libglew; make;

dirs:
	mkdir obj
	mkdir bin

clean:
	- rm -f ${OBJ} ${EXECUTABLE} deps
	- rm -rf ${BUNDLE}

ifeq "${INCLUDEDEPS}" ""
include deps
endif
