/*
  prog1.cpp
  Matthew Jee
  mcjee@ucsc.edu

  Putting together all the pieces of this wondrous machine!
*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wunused-comparison"
#include "libglui/glui.h"
#pragma clang diagnostic pop
#include "mjutil.h"
#include "mjshader.h"
#include "mjrenderable.h"
#include "mjmath.h"
#include "mjscene.h"
#include "mjprimitive.h"
#include "mjfluid.h"
#include <cmath>
#include <iostream>

using namespace mcjee;
using namespace std;

// --- Constants ---

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 640;

class Model : public Renderable {
public:
    Model(Geometry *geo, Shader *shader, GLenum drawType) :
        Renderable(geo, shader, drawType) {}
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // HACKY use same vertices for texture coordinates
        loc = shader->getAttribLocation("vTexCoord");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {

    }
};

static Shader *shader;
static Geometry *geo;
static Model *model;
static Scene *scene;
static FluidSolver *solver;
static GLuint *textureData;
static GLuint texture;
static int width = 20;
static int mainWindow;

static int lastX = 0;
static int lastY = 0;

void mouseMove(int x, int y) {
    if (-y+lastY == 0 && -x+lastX == 0) return;
    Vector3 rotationAxis(y-lastY, x-lastX, 0.0);
    float rotationAmount = rotationAxis.length();
    rotationAxis.normalize();

    model->rotate(rotationAmount, rotationAxis);
    lastX = x;
    lastY = y;
}

void mouseEvent(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    }
}

void render(void) {
    GLUI_Master.auto_set_viewport();

    solver->addDensity(1, 1, 1, 10.0);
    solver->solve(0.02);
    solver->fillDensityData(textureData);

    glBindTexture(GL_TEXTURE_3D, texture);
    glTexSubImage3D(GL_TEXTURE_3D,
                    0,
                    0, 0, 0,
                    width, width, width,
                    GL_RGBA,
                    GL_UNSIGNED_INT_8_8_8_8,
                    textureData);
    scene->render();
    glutSwapBuffers();
}

void idle(void) {
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void init(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_3D);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shader = new Shader();
    try {
        shader->compileProgram("shaders/shader.vsh",
                               "shaders/shader.fsh");
    } catch (mcjee::ShaderError &e) {
        cout << e.what() << endl;
    }

    scene = new Scene();

    scene->camera.perspective(-1.0f, 1.0f, -1.0f, 1.0f, 4.0f, 10.0f);
    scene->camera.translate(0.0, 3.0, 5.0);
    scene->camera.rotate(-30, 0, 0);

    float vertexData[cubeSize()];
    GLuint indexData[36];
    cubeVertices(vertexData, 1.0, 1.0, 1.0);
    cubeIndices(indexData);
    geo = new Geometry(vertexData, indexData, 8, 36, 3);

    model = new Model(geo, shader, GL_TRIANGLES);
    model->init();
    model->center = Vector3(0.5, 0.5, 0.5);

    scene->add(model);

    textureData = new GLuint[width*width*width];
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < width; ++k) {
                float facg = j*1.0/(width);
                int g = 0xff*facg;
                float facr = i*1.0/(width);
                int r = 0xff*facr;
                float facb = k*1.0/(width);
                int b = 0xff*facb;
                int a = 0xff*(facr*facg*facb);
                a = a > 0x22 ? 0xff : 0;
                GLuint f = a | (b << 8) | (g << 16) | (r << 24);
                textureData[i+j*width+k*width*width] = f;
            }
        }
    }
    //shader->use();
    GLint texLoc = shader->getUniformLocation("texture0");
    glUniform1i(texLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA,
                 width, width, width,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8,
                 textureData);

    solver = new FluidSolver(width, width, width);
}

void reshapeWindow(int, int) {
    GLUI_Master.reshape();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Fluid Simulator");

#ifndef __APPLE__
    glewInit();
#endif
    init();

    GLUI *gui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_BOTTOM);
    gui->add_statictext("le GUI");
    gui->add_column();

    glutDisplayFunc(render);
    glutMotionFunc(mouseMove);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutMouseFunc(mouseEvent);
    GLUI_Master.set_glutReshapeFunc(reshapeWindow);
    glutMainLoop();

    delete shader;
    return 0;
}
