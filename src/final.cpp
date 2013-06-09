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
#include <cmath>
#include <iostream>

using namespace mcjee;
using namespace std;

// --- Constants ---

static const int FRAME_WIDTH = 640;
static const int FRAME_HEIGHT = 640;
static int sidebarWidth;
static int sidebarHeight;
static int currentFrameWidth;
static int currentFrameHeight;

static int shaderIndex;
static Shader *shaders[3];
static Vector4 backgroundColors[] = {
    Vector4(1.0, 1.0, 1.0, 1.0),
    Vector4(0.0, 0.0, 0.0, 0.0),
    Vector4(0.0, 0.0, 0.0, 0.0),
};
static Shader *displayShader;
static Shader *simpleShader;
static FluidModel *fluidDomain;
static BoundingBox *boundingBox;
static ParticleSystem *particleSystem;
static Scene *scene;
static FluidSolver *solver;
static int width = 48;
static int mainWindow;
static Profiler *profiler;

static Texture2D *colorTarget;
static Framebuffer *mainFrameBuffer;
static Texture3D *densityTexture;
static uint16_t *densityTextureData;

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

    Matrix4 inverseModelViewMatrix = fluidDomain->inverseModelMatrix() * scene->camera.inverseViewMatrix();
    return inverseModelViewMatrix * Vector3(worldx, -worldy, -in.z);;
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
        fillPos = screenSpaceToFluidSpace(Vector3(x, y, scene->camera.position.z));
        Vector3 lastFillPos = screenSpaceToFluidSpace(Vector3(lastX, lastY, scene->camera.position.z));
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

void render(void) {
    profiler->start("ui");
    static long ox = 0;
    long x = glutGet(GLUT_ELAPSED_TIME);
    float dt = (x-ox)/1000.0;
    ox = x;
    float angle = x*0.005;
    float beta = x*0.006+1;
    int zz = sinf(beta)*width/4;
    Vector3 texSpaceFillPos = Vector3(fillPos.x*width, fillPos.y*width, fillPos.z*width);
    if (rightDown) {
        solver->addVelocity(texSpaceFillPos, fillVel*velocityScale);
        float vx = -sinf(angle)*3200.0;
        float vy = -cosf(angle)*3200.0;
        float g = vx < 0 ? 0 : vx/200;
        float b = vy < 0 ? 0 : vy/200;
        float r = zz*32 < 0 ? 0 : zz*3;
        solver->addDensity(texSpaceFillPos, Vector4(r, g, b, 10)*densityScale);
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
    solver->fillVelocityData(particleSystem->velocityBuffer);
    
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    solver->fillDensityData((uint16_t *)0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    densityTexture->initData((uint16_t *)0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    profiler->end("transfer voxels");

    profiler->start("render");
    glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
    glEnable(GL_CULL_FACE);

    particleSystem->update(dt); // do this during async pixel transfer
    particleSystem->rotation = fluidDomain->rotation;

    // composite pass
    fluidDomain->shader = shaders[shaderIndex];
    mainFrameBuffer->backgroundColor = backgroundColors[shaderIndex];
    glCullFace(GL_BACK);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    scene->render();

    //present framebuffer
    GLUI_Master.auto_set_viewport();
    colorTarget->borderColor(backgroundColors[shaderIndex]);
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
    colorTarget = new Texture2D(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, FRAME_WIDTH, FRAME_HEIGHT);
    colorTarget->interpolation(GL_LINEAR);
    colorTarget->initData((float *)0);

    mainFrameBuffer = new Framebuffer(FRAME_WIDTH, FRAME_HEIGHT);
    mainFrameBuffer->addRenderTarget(colorTarget, GL_COLOR_ATTACHMENT0);
    mainFrameBuffer->addRenderTarget(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
    mainFrameBuffer->backgroundColor = Vector4(0.0, 0.0, 0.0, 0.0);

    scene = new Scene(mainFrameBuffer);
    scene->camera.perspective(-1.0f, 1.0f, -1.0f, 1.0f, 8.0f, 20.0f);
    scene->camera.position = Vector3(0.0, 0.0, 10.0);
    scene->camera.zoom = 0.8;
    scene->blendEnabled = true;

    densityTextureData = new uint16_t[width*width*width*4];
    memset(densityTextureData, 0, sizeof(uint16_t)*width*width*width*4);
    densityTexture = new Texture3D(GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT, width, width, width);
    densityTexture->interpolation(GL_LINEAR);
    densityTexture->initData(densityTextureData);

    fluidDomain = new FluidModel(loadCube(1.0, 1.0, 1.0), shaders[0], GL_TRIANGLES);
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

    particleSystem = new ParticleSystem(simpleShader, width, width, width);
    particleSystem->center = Vector3(0.5, 0.5, 0.5);
    particleSystem->scaleUniform(1.5);
    particleSystem->init();
    scene->add(particleSystem);

    for (int i = 0; i < 5000; ++i) {
        particleSystem->add(Vector3(0.5+0.001*i, 0.5+0.001*i, 0.5+0.001*i), 1.0);
    }

    solver = new GPUSolver(width, width, width);

    fillPos = Vector3(0, 0, 0);

    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, width*width*width*4*sizeof(uint16_t), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);    
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

void eraseFluid() {
    glutSetWindow(mainWindow);
    solver->clearDensity();
    solver->clearVelocity();
}

void setInterpolation(bool on) {
    glutSetWindow(mainWindow);
    if (on)
        densityTexture->interpolation(GL_LINEAR);
    else
        densityTexture->interpolation(GL_NEAREST);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(FRAME_WIDTH, FRAME_HEIGHT);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Fluid Simulator");

#ifndef __APPLE__
    glewInit();
#endif
    compileShaders();
    init();

    uiInitialize(mainWindow);
    uiSetIterationsPointer(&solver->iterations);
    uiSetSamplesPointer(&fluidDomain->samples);
    uiSetShaderIndexPointer(&shaderIndex);
    uiSetVelocityScalePointer(&velocityScale);
    uiSetDensityScalePointer(&densityScale);
    uiSetInterpolationCallback(setInterpolation);
    uiSetEraseFluidCallback(eraseFluid);

    profiler = new Profiler(uiGetGLUI());
    profiler->addProfile("solve density");
    profiler->addProfile("solve velocity");
    profiler->addProfile("render");
    profiler->addProfile("ui");
    profiler->addProfile("transfer voxels");
    profiler->addProfile("total");

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

    //delete all shaders too
    //also somehow delete geometries which we dont have a handle on anymore
    uiTearDown();
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
