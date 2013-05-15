/*
  final.cpp
  Matthew Jee
  mcjee@ucsc.edu
*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wunused-comparison"
#include "libglui/glui.h"
#pragma clang diagnostic pop
#include "mj.h"
#include "mjfluid.h"
#include "mjhybridsolver.h"
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

static Shader *displayShader;
static Shader *smokeShader;
static Geometry *fluidDomainGeo;
static Model *fluidDomain;
static Scene *scene;
static FluidSolver *solver;
static int width = 32; // power of 2 plz
static int mainWindow;
static GLUI_StaticText *computeTimeText;
static char computeTimeString[16];
static GLuint windowFramebuffer;

static Texture2D *colorTarget;
static Framebuffer *mainFrameBuffer;
static Texture3D *densityTexture;
static GLfloat *densityTextureData;

static int lastX = 0;
static int lastY = 0;
static bool filling = false;

void keyboardEvent(unsigned char key, int, int) {
    switch(key) {
        case 32:
            filling = true;
            break;
        default: break;
    }
}

void keyboardUpEvent(unsigned char key, int, int) {
    switch(key) {
        case 32:
            filling = false;
            break;
        default: break;
    }
}

void mouseMove(int x, int y) {
    if (-y+lastY == 0 && -x+lastX == 0) return;
    Vector3 rotationAxis(y-lastY, x-lastX, 0.0);
    float rotationAmount = rotationAxis.length();
    rotationAxis.normalize();
    fluidDomain->rotateGlobal(rotationAmount, rotationAxis);
    lastX = x;
    lastY = y;
}

void mouseEvent(int, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    }
}

void render(void) {
    static long ox = 0;
    long x = glutGet(GLUT_ELAPSED_TIME);
    sprintf(computeTimeString, "%ldms", x-ox);
    computeTimeText->set_text(computeTimeString);
    computeTimeText->draw_text();
    float dt = (x-ox)/1000.0;
    ox = x;

    float angle = x*0.005;
    float beta = x*0.006+1;
    if (filling) {
        int xx = cosf(angle)*width/4;
        int yy = sinf(angle)*width/4;
        int zz = sinf(beta)*width/4;
        float vx = -sinf(angle)*100.0;
        float vy = -cosf(angle)*100.0;
        solver->addVelocityX(width/2+xx, width/2+yy, width/2+zz, vx);
        solver->addVelocityY(width/2+xx, width/2+yy, width/2+zz, vy);
        solver->addVelocityZ(width/2+xx, width/2+yy, width/2+zz, vy);
        solver->addDensity(width/2+xx, width/2+yy, width/2+zz, 20.0);
    }

    solver->solve(dt);
    solver->fillDensityData(densityTextureData);

    densityTexture->initData(densityTextureData);
    GLUI_Master.auto_set_viewport();

    scene->render();
    colorTarget->present(displayShader);
    glutSwapBuffers();
}

void idle(void) {
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void compileShaders(void) {
    smokeShader = new Shader();
    displayShader = new Shader();
    try {
        smokeShader->compile("shaders/shader.vsh", "shaders/shader.fsh");
        displayShader->compile("shaders/display.vsh", "shaders/display.fsh");
    } catch (mcjee::ShaderError &e) {
        cout << e.what() << endl;
    }
}

void init(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    colorTarget = new Texture2D(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, WINDOW_WIDTH, WINDOW_HEIGHT);
    colorTarget->initData((float *)0);

    mainFrameBuffer = new Framebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    mainFrameBuffer->addRenderTarget(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);

    scene = new Scene(mainFrameBuffer);
    scene->camera.perspective(-1.0f, 1.0f, -1.0f, 1.0f, 4.0f, 10.0f);
    scene->camera.translate(0.0, 2.2, 4.0);
    scene->camera.rotate(-30, 0, 0);

    float vertexData[cubeVerticesSize()];
    GLuint indexData[cubeIndicesSize()];
    cubeVertices(vertexData, 1.0, 1.0, 1.0);
    cubeIndices(indexData);
    fluidDomainGeo = new Geometry(vertexData, indexData, 8, 36, 3);

    densityTextureData = new GLfloat[width*width*width*3];
    densityTexture = new Texture3D(GL_RGB, GL_RGB, GL_FLOAT, width, width, width);
    densityTexture->interpolation(GL_LINEAR);
    densityTexture->initData(densityTextureData);

    fluidDomain = new Model(fluidDomainGeo, smokeShader, GL_TRIANGLES);
    fluidDomain->init();
    fluidDomain->center = Vector3(0.5, 0.5, 0.5);
    scene->add(fluidDomain);

    solver = new HybridSolver(width, width, width);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&windowFramebuffer);
}

void reshapeWindow(int, int) {
    GLUI_Master.reshape();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Fluid Simulator");

#ifndef __APPLE__
    glewInit();
#endif
    compileShaders();
    init();

    GLUI *gui = GLUI_Master.create_glui("Tools");
    gui->add_statictext("le GUI");
    computeTimeText = gui->add_statictext(computeTimeString);
    gui->add_column();

    glutDisplayFunc(render);
    glutMotionFunc(mouseMove);
    glutKeyboardUpFunc(keyboardUpEvent);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutMouseFunc(mouseEvent);
    GLUI_Master.set_glutReshapeFunc(reshapeWindow);
    GLUI_Master.set_glutKeyboardFunc(keyboardEvent);
    glutMainLoop();

    delete smokeShader;
    return 0;
}
