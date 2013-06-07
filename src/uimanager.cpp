/*
    uimanager.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "uimanager.h"

struct {
    int _shaderIndex; int *shaderIndex;
    int _samples; int *samples;
    int _iterations; int *iterations;
    float _velocityMultiplier; float *velocityMultiplier;
    float _densityMultiplier; float *densityMultiplier;
} data;

static void shaderListCallback(GLUI_Control *) {
    *data.shaderIndex = data._shaderIndex;
}

static void samplesSpinnerCallback(GLUI_Control *) {
    *data.samples = data._samples;
}

void uiInitialize(int window) {
    GLUI *gui = GLUI_Master.create_glui_subwindow(window, GLUI_SUBWINDOW_LEFT);
    GLUI_Panel *simulationPanel = gui->add_panel("Simulation");
    GLUI_Control *control = gui->add_spinner_to_panel(simulationPanel, "Velocity Scale");
    control->set_alignment(GLUI_ALIGN_RIGHT);
    control = gui->add_spinner_to_panel(simulationPanel, "Density Scale");
    control->set_alignment(GLUI_ALIGN_RIGHT);
    control = gui->add_spinner_to_panel(simulationPanel, "Iterations");
    control->set_alignment(GLUI_ALIGN_RIGHT);
    gui->add_button_to_panel(simulationPanel, "Erase Fluid");

    GLUI_Panel *renderingPanel = gui->add_panel("Rendering");
    control = gui->add_spinner_to_panel(renderingPanel, "Samples", GLUI_SPINNER_INT, &(data._samples), -1, samplesSpinnerCallback);
    control->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Listbox *listBox = gui->add_listbox_to_panel(renderingPanel, "Shading ", &(data._shaderIndex), -1, shaderListCallback);
    listBox->set_alignment(GLUI_ALIGN_RIGHT);
    listBox->add_item(0, "Smoke");
    listBox->add_item(1, "Colored Smoke");
    listBox->add_item(2, "Glow");
    listBox->add_item(3, "Fire");
    gui->add_checkbox_to_panel(renderingPanel, "Interpolation");
}

void uiTearDown() {

}

void uiSetSamplesPointer(int *p) {
    data.samples = p;
}

void uiSetShaderIndexPointer(int *p) {
    data.shaderIndex = p;
}

void uiSetIterationsPointer(int *p) {

}

void uiSetVelocityMultiplierPointer(int *p) {

}

void uiSetDensityMultiplierPointer(int *p) {

}

