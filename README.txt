Program 1
Matthew Jee
mcjee@ucsc.edu

Platforms:
    Mac OS X, some Linux. Tested on OS X 10.8.
    Executables are in the bin directory.

Build Instructions:

    TLDR: run `make` in the src directory.

    Xcode 4 and the developer command line tools must be installed.

    Open up a terminal, got to the src directory, and `make`. This
    will create a binary, which is runnable from a terminal, as well
    as Terrain.app, which can be opened from Finder. Terrain.app
    bundles all of the shaders needed to run.

    If the build isn't working on an older machine for some reason,
    then maybe all of the references to clang in the Makefile
    should be changed to gcc.

    To do this, change line 28 from:
        COMPILER = clang++
    to
        COMPILER = g++

Source Files:
    prog1.cpp
        Program entry point. Contains all of the GUI stuff and brings
        everything else together.

    terrainmodel.h
        Contains a definition of a subclass of renderable (see below)
        to render the terrain vertex data.

    mjutil.h/cpp
        Utilities to cover up apple specific extensions,
        and timing things.

    mjterrain.h/cpp
        Defines the class Terrain, used to do all of the fancy terrain
        generation.

    mjshader.h/cpp
        Defines the class Shader, a convenience wrapper for OpenGL
        shader compilation and linkage calls.

    mjrenderable.h/cpp
        Defines the abstract class Renderable, a convenience wrapper
        for rendering vertex arrays with VBOs, VAOs and simple
        shaders.

    terrainShader.vsh/fsh
        Shaders to make the terrain change colors!
