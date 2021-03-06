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
#include "half.h"
#include "colors.h"
#include <cmath>
#include <iostream>
#include <CoreFoundation/CoreFoundation.h>

using namespace mcjee;
using namespace std;

static const int FRAME_WIDTH = 640;
static const int FRAME_HEIGHT = 640;
static int sidebarWidth;
static int sidebarHeight;
static int currentFrameWidth;
static int currentFrameHeight;

static Shader *displayShader;
static Shader *simpleShader;
static Shader *particleShader;
static Shader *glowShader;
static Shader *mixShader;
static FluidModel *fluidDomain;
static BoundingBox *boundingBox;
static ParticleSystem *particleSystem;
static Scene *scene;
static FluidSolver *solver;
static int width = 64; //works up to 90 on 330m
static int mainWindow;
static Profiler *profiler;

static Texture2D *colorTarget;
static Framebuffer *mainFrameBuffer;

static bool fullscreen;
static int lastX = 0;
static int lastY = 0;
static Vector3 fillPos;
static Vector3 fillVel;
static bool leftDown;
static bool rightDown;
static float velocityScale;
static float densityScale;

static GLuint pbo;

// OPTIONS
static int colorFuncIndex;
static int glow;
static int darkBackground;

// converts from a screen space coordinate to a
// world space coordinate on the screen space xy plane where world z = 0
// the converts to density texture space
Vector3 screenSpaceToFluidSpace(Vector3 in) {
    float scale = 1.0*currentFrameHeight/FRAME_HEIGHT;
    int viewportX = (in.x-sidebarWidth-(currentFrameWidth-FRAME_WIDTH*scale)/2)/scale;
    int viewportY = (in.y-sidebarHeight)/scale;

    //the depth of global plane z=0 in view space
    float n = 8.0*scene->camera.zoom; // FIXIT TODO BLAH 8.0 is scene camera near
    float f = in.z;
    float worldx = (viewportX*2.0/FRAME_WIDTH-1.0)*f/n;
    float worldy = (viewportY*2.0/FRAME_HEIGHT-1.0)*f/n;

    Matrix4 inverseModelMatrix = fluidDomain->inverseModelMatrix();
    Matrix4 inverseViewMatrix = scene->camera.inverseViewMatrix();

    Matrix4 inverseModelViewMatrix = inverseModelMatrix*inverseViewMatrix;
    return inverseModelViewMatrix * Vector3(worldx, -worldy, -in.z);;
}

void render(void) {
    profiler->start("ui");
    static long ox = 0;
    long x = glutGet(GLUT_ELAPSED_TIME);
    float dt = (x-ox)/1000.0;
    ox = x;

    Vector3 texSpaceFillPos = Vector3(fillPos.x*width, fillPos.y*width, fillPos.z*width);
    if (rightDown) {
        Vector4 (*f)(float) = getColorFunc(colorFuncIndex);
        solver->addVelocity(texSpaceFillPos, fillVel*velocityScale);
        float alpha = x*0.01;
        solver->addDensity(texSpaceFillPos, f(alpha)*densityScale);
    }
    dumpGLError();
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
    solver->fillVelocityData(particleSystem->velocityBuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    solver->fillDensityData((uint16_t *)0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    fluidDomain->densityTexture->initData((uint16_t *)0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    profiler->end("transfer voxels");

    profiler->start("render");
    glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    particleSystem->update(dt); // do this during async pixel transfer
    particleSystem->rotation = fluidDomain->rotation;

    if (glow) fluidDomain->shader = glowShader;
    else fluidDomain->shader = mixShader;
    if (darkBackground) mainFrameBuffer->backgroundColor = Vector4();
    else mainFrameBuffer->backgroundColor = Vector4(1.0, 1.0, 1.0, 1.0);

    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    scene->render();

    //present framebuffer
    GLUI_Master.auto_set_viewport();
    colorTarget->borderColor(mainFrameBuffer->backgroundColor);
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
    particleShader = new Shader();
    mixShader = new Shader();
    glowShader = new Shader();
    try {
        mixShader->compile("shaders/colorBlend.vsh", "shaders/colorBlend.fsh");
        glowShader->compile("shaders/colorBlend.vsh", "shaders/colorAdd.fsh");
        displayShader->compile("shaders/display.vsh", "shaders/display.fsh");
        simpleShader->compile("shaders/simple.vsh", "shaders/simple.fsh");
        particleShader->compile("shaders/simple.vsh", "shaders/particle.fsh");
    } catch (mcjee::ShaderError &e) {
        cout << e.what() << endl;
    }
}

void initSimulation(int size) {
    scene = new Scene(mainFrameBuffer);
    scene->camera.perspective(-1.0f, 1.0f, -1.0f, 1.0f, 8.0f, 20.0f);
    scene->camera.position = Vector3(0.0, 0.0, 10.0);
    scene->camera.zoom = 0.8;
    scene->blendEnabled = true;

    fluidDomain = new FluidModel(mixShader, GL_TRIANGLES, size, size, size);
    fluidDomain->init();
    fluidDomain->center = Vector3(0.5, 0.5, 0.5);
    fluidDomain->scaleUniform(1.5);
    scene->add(fluidDomain);

    boundingBox = new BoundingBox(simpleShader, fluidDomain);
    boundingBox->init();
    scene->add(boundingBox);

    particleSystem = new ParticleSystem(particleShader, size, size, size);
    particleSystem->center = Vector3(0.5, 0.5, 0.5);
    particleSystem->scaleUniform(1.5);
    particleSystem->init();
    scene->add(particleSystem);

    for (int i = 0; i < 5000; ++i) {
        particleSystem->add(Vector3(), 1.0);
    }

    solver = new GPUSolver(size, size, size);

    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, size*size*size*4*sizeof(uint16_t), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    uiSetIterationsPointer(&solver->iterations);
    uiSetSamplesPointer(&fluidDomain->samples);
    uiSetParticlesEnabledPointer(&particleSystem->visible);
    uiSetBoundingBoxPointer(&boundingBox->visible);
}

void deleteSimulation() {
    delete scene;
    delete fluidDomain;
    delete boundingBox;
    delete particleSystem;
    delete solver;
    glDeleteBuffers(1, &pbo);
}

void reshapeWindow(int, int) {
    GLUI_Master.reshape();
    int vx, vy;
    GLUI_Master.get_viewport_area(&vx,
                                  &vy,
                                  &currentFrameWidth,
                                  &currentFrameHeight);
    GLint loc = displayShader->getUniformLocation("aspectRatio");
    displayShader->use();
    glUniform1f(loc, 1.0*currentFrameWidth/currentFrameHeight);
}

void keyboardEvent(unsigned char, int, int) {
    fullscreen = !fullscreen;
    if (fullscreen) glutFullScreen();
    else glutReshapeWindow(640+sidebarWidth, 640+sidebarHeight);
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
        //transform to frame coordinates
        Vector3 curPos = Vector3(x, y, scene->camera.position.z);
        Vector3 lastPos = Vector3(lastX, lastY, scene->camera.position.z);
        fillPos = screenSpaceToFluidSpace(curPos);
        Vector3 lastFillPos = screenSpaceToFluidSpace(lastPos);
        fillVel = fillPos-lastFillPos;
        if (fillPos.x > 1.0) fillPos.x = 1.0;
        if (fillPos.x < 0) fillPos.x = 0;
        if (fillPos.y > 1.0) fillPos.y = 1.0;
        if (fillPos.y < 0) fillPos.y = 0;
        if (fillPos.z > 1.0) fillPos.z = 1.0;
        if (fillPos.z < 0) fillPos.z = 0;
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

void changeSimulationSize(int newSize) {
        glutSetWindow(mainWindow);
    deleteSimulation();
    width = newSize;
    initSimulation(width);
}

void eraseFluid() {
    glutSetWindow(mainWindow);
    solver->clearDensity();
    solver->clearVelocity();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(FRAME_WIDTH, FRAME_HEIGHT);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Fluid Simulator");
    //for debug with Instruments
    //Core Foundation needs to be used somewhere
    //CFShow(CFSTR("Debug Mode\n"));
#ifndef __APPLE__
    glewInit();
#endif
    compileShaders();
    
    colorTarget = new Texture2D(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, FRAME_WIDTH, FRAME_HEIGHT);
    colorTarget->interpolation(GL_LINEAR);
    colorTarget->initData((float *)0);

    mainFrameBuffer = new Framebuffer(FRAME_WIDTH, FRAME_HEIGHT);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    mainFrameBuffer->addRenderTarget(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
    mainFrameBuffer->backgroundColor = Vector4(0.0, 0.0, 0.0, 0.0);

    GLUI *gui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_LEFT);
    profiler = new Profiler(gui);
    profiler->addProfile("solve density");
    profiler->addProfile("solve velocity");
    profiler->addProfile("render");
    profiler->addProfile("ui");
    profiler->addProfile("transfer voxels");
    profiler->addProfile("total");
    uiInitialize(gui);
    uiSetShaderIndexPointer(&colorFuncIndex);
    uiSetVelocityScalePointer(&velocityScale);
    uiSetDensityScalePointer(&densityScale);
    uiSetGlowPointer(&glow);
    uiSetDarkBackgroundPointer(&darkBackground);
    uiSetEraseFluidCallback(eraseFluid);
    uiSetSubdivisionsCallback(changeSimulationSize);

    initSimulation(width);

    int vx, vy, vw, vh;
    GLUI_Master.get_viewport_area(&vx, &vy, &vw, &vh);
    sidebarWidth = FRAME_WIDTH-vw;
    sidebarHeight = FRAME_HEIGHT-vh;
    glutReshapeWindow(FRAME_WIDTH+sidebarWidth, FRAME_HEIGHT);

    glutDisplayFunc(render);
    glutMotionFunc(mouseMove);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutMouseFunc(mouseEvent);
    GLUI_Master.set_glutReshapeFunc(reshapeWindow);
    GLUI_Master.set_glutKeyboardFunc(keyboardEvent);
    glutMainLoop();

    uiTearDown();
    deleteSimulation();
    delete displayShader;
    delete simpleShader;
    delete particleShader;
    delete glowShader;
    delete mixShader;
    delete colorTarget;
    delete mainFrameBuffer;
    delete profiler;
    return 0;
}
