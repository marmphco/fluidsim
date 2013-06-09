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

void uiSetEraseFluidCallback(void (*)());
void uiSetInterpolationCallback(void (*)(bool));
void uiSetParticlesEnabledCallback(void (*)(bool));

#endif
