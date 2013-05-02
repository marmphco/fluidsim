UNAME = ${shell uname}
ARCH = ${shell uname -m}
all: glew.o glewinfo.o visualinfo.o
	gcc -c glew.c
	gcc -c glewinfo.c
	gcc -c visualinfo.c
	ar crs libglew${UNAME}${ARCH}.a glew.o glewinfo.o visualinfo.o

