/*
    uimanager.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "mj.h"

using namespace mcjee;

void uiInitialize(int window);
void uiTearDown();

void uiSetSamplesPointer(int *);
void uiSetShaderIndexPointer(int *);
void uiSetIterationsPointer(int *);
void uiSetVelocityMultiplierPointer(int *);
void uiSetDensityMultiplierPointer(int *);

#endif