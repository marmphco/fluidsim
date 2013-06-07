/*
    final.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/
#include "mj.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wunused-comparison"
#include "libglui/glui.h"
#pragma clang diagnostic pop
#include "mjfluid.h"
#include "mjgpusolver.h"
#include "models.h"
#include "uimanager.h"
#include <cmath>
#include <iostream>

using namespace mcjee;
using namespace std;

// --- Constants ---

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 640;

static int shaderIndex;
static Shader *shaders[3];
static Vector4 backgroundColors[] = {
    Vector4(1.0, 1.0, 1.0, 1.0),
    Vector4(0.0, 0.0, 0.0, 0.0),
    Vector4(0.0, 0.0, 0.0, 0.0),
};
static Shader *displayShader;
static Shader *simpleShader;
static Geometry *fluidDomainGeo;
static FluidModel *fluidDomain;
static BoundingBox *boundingBox;
static Scene *scene;
static FluidSolver *solver;
static int width = 64;
static int mainWindow;
static GLuint windowFramebuffer;
static Profiler *profiler;

static Texture2D *colorTarget;
static Framebuffer *mainFrameBuffer;
static Texture3D *densityTexture;
static GLfloat *densityTextureData;

static int lastX = 0;
static int lastY = 0;
static bool filling = false;

static Vector3 fillPos;
static Vector3 fillVel;
static bool leftDown;
static bool rightDown;

static GLuint pbo;

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
    if (leftDown) {
        Vector3 rotationAxis(y-lastY, x-lastX, 0.0);
        float rotationAmount = rotationAxis.length();
        rotationAxis.normalize();
        fluidDomain->rotateGlobal(rotationAmount, rotationAxis);
    }
    if (rightDown) {
        fillVel = Vector3(x-lastX, y-lastY, 0);
        fillPos += fillVel;
        if (fillPos.x > WINDOW_WIDTH) {
            fillPos.x = WINDOW_WIDTH;
        }
        if (fillPos.x < 0) {
            fillPos.x = 0;
        }
        if (fillPos.y > WINDOW_HEIGHT) {
            fillPos.y = WINDOW_HEIGHT;
        }
        if (fillPos.y < 0) {
            fillPos.y = 0;
        }
    }
    lastX = x;
    lastY = y;
}

void mouseEvent(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
        if (button == GLUT_LEFT_BUTTON) leftDown = true;
        if (button == GLUT_RIGHT_BUTTON) rightDown = true;
    } else {
        if (button == GLUT_LEFT_BUTTON) leftDown = false;
        if (button == GLUT_RIGHT_BUTTON) rightDown = false;
    }
}

void render(void) {
    profiler->start("ui");
    static long ox = 0;
    long x = glutGet(GLUT_ELAPSED_TIME);
    float dt = (x-ox)/1000.0;
    ox = x;
    float angle = x*0.005;
    float beta = x*0.006+1;
    int xx = cosf(angle)*width/4;
    int yy = sinf(angle)*width/4;
    int zz = sinf(beta)*width/4;
    fillVel.z = zz;
    solver->addVelocity(Vector3(fillPos.x*width/640, fillPos.y*width/640, width/2), fillVel*100);
    if (filling) {

        float vx = -sinf(angle)*3200.0;
        float vy = -cosf(angle)*3200.0;
        float g = vx < 0 ? 0 : vx/200;
        float b = vy < 0 ? 0 : vy/200;
        float r = zz*32 < 0 ? 0 : zz*3;
        solver->addDensity(Vector3(fillPos.x*width/640, fillPos.y*width/640, width/2), Vector3(r, g, b));
    }

    profiler->end("ui");
    profiler->end("total");
    profiler->start("total");

    profiler->start("solve density");
    solver->solveDensities(dt);
    profiler->end("solve density");

    profiler->start("solve velocity");
    solver->solveVelocities(dt);
    profiler->end("solve velocity");

    profiler->start("transfer voxels");
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    solver->fillDensityData((float *)0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    densityTexture->initData((float *)0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    profiler->end("transfer voxels");

    profiler->start("render");
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_CULL_FACE);

    // composite pass
    fluidDomain->shader = shaders[shaderIndex];
    mainFrameBuffer->backgroundColor = backgroundColors[shaderIndex];
    glCullFace(GL_BACK);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    scene->sFactorRGB = GL_SRC_ALPHA;
    scene->dFactorRGB = GL_ONE_MINUS_SRC_ALPHA;
    scene->sFactorA = GL_SRC_ALPHA;
    scene->dFactorA = GL_ONE_MINUS_SRC_ALPHA;
    scene->blendEquationA = GL_FUNC_ADD;
    scene->render();

    //present framebuffer
    GLUI_Master.auto_set_viewport();
    colorTarget->present(displayShader);
    glutSwapBuffers();
    profiler->end("render");
}

void idle(void) {
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void compileShaders(void) {
    displayShader = new Shader();
    simpleShader = new Shader();
    shaders[0] = new Shader();
    shaders[1] = new Shader();
    shaders[2] = new Shader();
    try {
        shaders[0]->compile("shaders/colorBlend.vsh", "shaders/colorBlend.fsh");
        shaders[1]->compile("shaders/colorBlend.vsh", "shaders/colorBlend.fsh");
        shaders[2]->compile("shaders/colorBlend.vsh", "shaders/colorAdd.fsh");
        displayShader->compile("shaders/display.vsh", "shaders/display.fsh");
        simpleShader->compile("shaders/simple.vsh", "shaders/simple.fsh");
    } catch (mcjee::ShaderError &e) {
        cout << e.what() << endl;
    }
}

void init(void) {
    colorTarget = new Texture2D(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, WINDOW_WIDTH, WINDOW_HEIGHT);
    colorTarget->initData((float *)0);

    mainFrameBuffer = new Framebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    mainFrameBuffer->addRenderTarget(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
    mainFrameBuffer->backgroundColor = Vector4(0.0, 0.0, 0.0, 0.0);

    scene = new Scene(mainFrameBuffer);
    scene->camera.perspective(-1.0f, 1.0f, -1.0f, 1.0f, 8.0f, 20.0f);
    scene->camera.position = Vector3(0.0, 0.0, 10.0);
    scene->camera.position = Vector3(0.0, 4.4, 8.0);
    scene->camera.rotateLocal(-30, X_AXIS);
    scene->camera.zoom = 0.8;
    scene->blendEnabled = true;

    fluidDomainGeo = loadCube(1.0, 1.0, 1.0);

    densityTextureData = new GLfloat[width*width*width*4];
    memset(densityTextureData, 0, sizeof(float)*width*width*width*4);
    densityTexture = new Texture3D(GL_RGBA, GL_RGBA, GL_FLOAT, width, width, width);
    densityTexture->interpolation(GL_LINEAR);
    densityTexture->initData(densityTextureData);

    fluidDomain = new FluidModel(fluidDomainGeo, shaders[0], GL_TRIANGLES);
    fluidDomain->init();
    fluidDomain->center = Vector3(0.5, 0.5, 0.5);
    fluidDomain->scaleUniform(1.5);
    fluidDomain->densityBuffer = densityTexture;
    scene->add(fluidDomain);

    boundingBox = new BoundingBox(loadWireCube(1.0, 1.0, 1.0), simpleShader, fluidDomain);
    boundingBox->init();
    boundingBox->center = Vector3(0.5, 0.5, 0.5);
    boundingBox->scaleUniform(1.5);
    scene->add(boundingBox);

    solver = new GPUSolver(width, width, width);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&windowFramebuffer);

    fillPos = Vector3(width/2, width/2, width/2);

    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, width*width*width*4*sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);    
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

    uiInitialize(mainWindow);
    uiSetSamplesPointer(&fluidDomain->samples);
    uiSetShaderIndexPointer(&shaderIndex);

    profiler = new Profiler();
    profiler->addProfile("solve density");
    profiler->addProfile("solve velocity");
    profiler->addProfile("render");
    profiler->addProfile("ui");
    profiler->addProfile("transfer voxels");
    profiler->addProfile("total");

    int vx, vy, vw, vh;
    GLUI_Master.get_viewport_area(&vx, &vy, &vw, &vh);
    glutReshapeWindow(WINDOW_WIDTH+(WINDOW_WIDTH-vw), WINDOW_HEIGHT);

    glutDisplayFunc(render);
    glutMotionFunc(mouseMove);
    glutKeyboardUpFunc(keyboardUpEvent);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutMouseFunc(mouseEvent);
    GLUI_Master.set_glutReshapeFunc(reshapeWindow);
    GLUI_Master.set_glutKeyboardFunc(keyboardEvent);
    glutMainLoop();

    //delete all shaders
    delete displayShader;
    delete colorTarget;
    delete mainFrameBuffer;
    delete scene;
    delete densityTextureData;
    delete densityTexture;
    delete fluidDomain;
    delete solver;
    delete profiler;
    return 0;
}
