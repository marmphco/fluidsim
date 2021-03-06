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

SUBMITNAME = proj
BUNDLENAME = FluidSim
EXECNAME = fluidSim
SRCDIR = src
SHADERDIR = shaders
OBJDIR = obj
BINDIR = bin

_CSRC = final.cpp mjshader.cpp mjrenderable.cpp\
        mjutil.cpp mjscene.cpp mjcamera.cpp mjlight.cpp\
        mjloader.cpp mjgeometry.cpp mjfluid.cpp\
        mjtexture.cpp mjmatrix.cpp mjvector.cpp mjframebuffer.cpp\
        mjgpusolver.cpp mjprofiler.cpp uimanager.cpp half.cpp\
        models.cpp colors.cpp

_SHADERS = colorBlend.fsh colorBlend.vsh colorAdd.fsh\
           display.vsh display.fsh\
           add.fsh advect.fsh divergence.fsh project2.fsh\
           subgradient.fsh kernel.vsh\
           simple.vsh simple.fsh particle.fsh

EXECUTABLE = ${BINDIR}/${EXECNAME}
BUNDLE = ${BINDIR}/${BUNDLENAME}.app

CSRC = ${patsubst %,${SRCDIR}/%,${_CSRC}}
OBJ = ${patsubst %.cpp,${OBJDIR}/%.o,${_CSRC}}
SHADERS = ${patsubst %,${SHADERDIR}/%,${_SHADERS}}

OPTIONS = -Wall -Wextra -ansi -pedantic
ifeq "${UNAME}" "Darwin"
# OS X/Darwin, using clang and stuff.
	COMPILER = clang++
	COMPILE = ${COMPILER} ${OPTIONS}
	FRAMEWORKS = -framework OpenGL -framework GLUT -framework CoreFoundation
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
	- rm -rf ${SUBMITNAME}
	- mkdir ${SUBMITNAME}\
	 ${SUBMITNAME}/code\
	 ${SUBMITNAME}/code/obj\
	 ${SUBMITNAME}/data
	cp README.txt ${SUBMITNAME}
	cp Makefile ${SUBMITNAME}/code
	cp -r src ${SUBMITNAME}/code
	cp -r bin ${SUBMITNAME}/code
	cp -r shaders ${SUBMITNAME}/code
	cp -r shaders ${SUBMITNAME}/code/bin
	cp -r report ${SUBMITNAME}
	cp favscreenshot1.png ${SUBMITNAME}
	cp favscreenshot2.png ${SUBMITNAME}
	#tar -cf ${SUBMITNAME}.tar ${SUBMITNAME}

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
	- rm -rf ${BINDIR}/*
	- rm -rf ${SUBMITNAME}

ifeq "${INCLUDEDEPS}" ""
include deps
endif
