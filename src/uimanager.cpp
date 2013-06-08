/*
    uimanager.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "uimanager.h"

static const float BASE_VELOCITY_SCALE = 50000.0;
static const float BASE_DENSITY_SCALE = 1.0;

struct {
    int _shaderIndex; int *shaderIndex;
    int _samples; int *samples;
    int _iterations; int *iterations;
    float _velocityScale; float *velocityScale;
    float _densityScale; float *densityScale;
    int _interpolation;
    void (*interpolationSwitch)(bool);
    void (*eraseFluid)();
} data;

//these should check for null
static void shaderList(GLUI_Control *) {
    *data.shaderIndex = data._shaderIndex;
}

static void samplesSpinner(GLUI_Control *) {
    *data.samples = data._samples;
}

static void velocityScaleSpinner(GLUI_Control *) {
    *data.velocityScale = BASE_VELOCITY_SCALE*data._velocityScale;
}

static void densityScaleSpinner(GLUI_Control *) {
    *data.densityScale = BASE_DENSITY_SCALE*data._densityScale;
}

static void iterationsSpinner(GLUI_Control *) {
    *data.iterations = data._iterations;
}

static void eraseFluidButton(GLUI_Control *) {
    data.eraseFluid();
}

static void interpolationCheckbox(GLUI_Control *) {
    data.interpolationSwitch(data._interpolation);
}

void uiInitialize(int window) {
    GLUI *gui = GLUI_Master.create_glui_subwindow(window,
                                                  GLUI_SUBWINDOW_LEFT);
    GLUI_Panel *simulationPanel = gui->add_panel("Simulation");
    GLUI_Control *control = gui->add_spinner_to_panel(simulationPanel,
                                                      "Velocity Scale",
                                                      GLUI_SPINNER_FLOAT,
                                                      &(data._velocityScale),
                                                      -1,
                                                      velocityScaleSpinner);
    control->set_float_val(1.0);
    control->set_alignment(GLUI_ALIGN_RIGHT);

    control = gui->add_spinner_to_panel(simulationPanel,
                                        "Density Scale",
                                        GLUI_SPINNER_FLOAT,
                                        &(data._densityScale),
                                        -1,
                                        densityScaleSpinner);
    control->set_float_val(1.0);
    control->set_alignment(GLUI_ALIGN_RIGHT);

    control = gui->add_spinner_to_panel(simulationPanel,
                                        "Iterations",
                                        GLUI_SPINNER_INT,
                                        &(data._iterations),
                                        -1,
                                        iterationsSpinner);
    control->set_alignment(GLUI_ALIGN_RIGHT);
    gui->add_button_to_panel(simulationPanel,
                             "Erase Fluid",
                             -1,
                             eraseFluidButton);

    GLUI_Panel *renderingPanel = gui->add_panel("Rendering");
    control = gui->add_spinner_to_panel(renderingPanel,
                                        "Samples",
                                        GLUI_SPINNER_INT,
                                        &(data._samples),
                                        -1,
                                        samplesSpinner);
    control->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Listbox *listBox = gui->add_listbox_to_panel(renderingPanel,
                                                      "Shading ",
                                                      &(data._shaderIndex),
                                                      -1,
                                                      shaderList);
    listBox->set_alignment(GLUI_ALIGN_RIGHT);
    listBox->add_item(0, "Smoke");
    listBox->add_item(1, "Colored Smoke");
    listBox->add_item(2, "Glow");
    listBox->add_item(3, "Fire");
    gui->add_checkbox_to_panel(renderingPanel,
                               "Interpolation",
                               &(data._interpolation),
                               -1,
                               interpolationCheckbox);
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
    data.iterations = p;
}

void uiSetVelocityScalePointer(float *p) {
    data.velocityScale = p;
    *data.velocityScale = BASE_VELOCITY_SCALE*data._velocityScale;
}

void uiSetDensityScalePointer(float *p) {
    data.densityScale = p;
    *data.densityScale = BASE_DENSITY_SCALE*data._densityScale;
}

void uiSetEraseFluidCallback(void (*callback)()) {
    data.eraseFluid = callback;
}

void uiSetInterpolationCallback(void (*callback)(bool)) {
    data.interpolationSwitch = callback;
}
