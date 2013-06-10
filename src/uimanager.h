/*
    uimanager.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "mj.h"

using namespace mcjee;

void uiInitialize(GLUI *gui);
void uiTearDown();

GLUI *uiGetGLUI();

void uiSetSamplesPointer(int *);
void uiSetShaderIndexPointer(int *);
void uiSetIterationsPointer(int *);
void uiSetVelocityScalePointer(float *);
void uiSetDensityScalePointer(float *);
void uiSetParticlesEnabledPointer(int *);
void uiSetGlowPointer(int *);
void uiSetDarkBackgroundPointer(int *);
void uiSetBoundingBoxPointer(int *);
void uiSetSubdivisionsCallback(void (*)(int));
void uiSetEraseFluidCallback(void (*)());

#endif
