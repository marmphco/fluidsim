/*
    uimanager.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "uimanager.h"

static const float BASE_VELOCITY_SCALE = 4000.0;
static const float BASE_DENSITY_SCALE = 1.0;

struct {
    GLUI *glui;
    int _shaderIndex; int *shaderIndex;
    int _samples; int *samples;
    int _iterations; int *iterations;
    float _velocityScale; float *velocityScale;
    float _densityScale; float *densityScale;
    int _particlesEnabled; int *particlesEnabled;
    int _glow; int *glow;
    int _darkBackground; int *darkBackground;
    int _boundingBox; int *boundingBox;
    int _subdivisions; void (*subdivisions)(int);
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

static void particlesEnabledCheckbox(GLUI_Control *) {
    *data.particlesEnabled = data._particlesEnabled;
}

static void glowCheckbox(GLUI_Control *) {
    *data.glow = data._glow;
}

static void darkBackgroundCheckbox(GLUI_Control *) {
    *data.darkBackground = data._darkBackground;
}

static void boundingBoxCheckbox(GLUI_Control *) {
    *data.boundingBox = data._boundingBox;
}

static void subdivisionsRadioGroup(GLUI_Control *) {
    int size = 32;
    switch(data._subdivisions) {
        case 0: size = 32; break;
        case 1: size = 48; break;
        case 2: size = 64; break;
    }
    data.subdivisions(size);
}

static void eraseFluidButton(GLUI_Control *) {
    data.eraseFluid();
}

void uiInitialize(GLUI *gui) {
    data.glui = gui;
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
    control->set_int_val(16);
    control->set_alignment(GLUI_ALIGN_RIGHT);

    gui->add_separator_to_panel(simulationPanel);
    gui->add_statictext_to_panel(simulationPanel, "Subdivisions");
    GLUI_RadioGroup *subGroup = gui->add_radiogroup_to_panel(simulationPanel,
                                                             &(data._subdivisions),
                                                             -1,
                                                             subdivisionsRadioGroup);
    gui->add_radiobutton_to_group(subGroup, "32 x 32 x 32");
    gui->add_radiobutton_to_group(subGroup, "48 x 48 x 48");
    gui->add_radiobutton_to_group(subGroup, "64 x 64 x 64");
    gui->add_separator_to_panel(simulationPanel);

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
    control->set_int_val(64);

    GLUI_Listbox *listBox = gui->add_listbox_to_panel(renderingPanel,
                                                      "Colors ",
                                                      &(data._shaderIndex),
                                                      -1,
                                                      shaderList);
    listBox->add_item(0, "Bluish");
    listBox->add_item(1, "Smoke");
    listBox->add_item(3, "Fire");
    listBox->add_item(4, "Rainbows");
    listBox->add_item(5, "Plasma");
    control = gui->add_checkbox_to_panel(renderingPanel,
                                         "Glow (Additive Blend)",
                                         &(data._glow),
                                         -1,
                                         glowCheckbox);
    control = gui->add_checkbox_to_panel(renderingPanel,
                                         "Dark Background",
                                         &(data._darkBackground),
                                         -1,
                                         darkBackgroundCheckbox);
    control->set_int_val(1);

    control = gui->add_checkbox_to_panel(renderingPanel,
                                         "Particles",
                                         &(data._particlesEnabled),
                                         -1,
                                         particlesEnabledCheckbox);
    control->set_int_val(1);
    control = gui->add_checkbox_to_panel(renderingPanel,
                                         "Bounding Box",
                                         &(data._boundingBox),
                                         -1,
                                         boundingBoxCheckbox);
    control->set_int_val(1);
}

void uiTearDown() {
    data.glui->close();
}

GLUI *uiGetGLUI() {
    return data.glui;
}

void uiSetSamplesPointer(int *p) {
    data.samples = p;
    *p = data._samples;
}

void uiSetShaderIndexPointer(int *p) {
    data.shaderIndex = p;
    *p = data._shaderIndex;
}

void uiSetIterationsPointer(int *p) {
    data.iterations = p;
    *p = data._iterations;
}

void uiSetVelocityScalePointer(float *p) {
    data.velocityScale = p;
    *data.velocityScale = BASE_VELOCITY_SCALE*data._velocityScale;
}

void uiSetDensityScalePointer(float *p) {
    data.densityScale = p;
    *data.densityScale = BASE_DENSITY_SCALE*data._densityScale;
}

void uiSetParticlesEnabledPointer(int *p) {
    data.particlesEnabled = p;
    *data.particlesEnabled = data._particlesEnabled;
}

void uiSetGlowPointer(int *p) {
    data.glow = p;
    *data.glow = data._glow;
}

void uiSetDarkBackgroundPointer(int *p) {
    data.darkBackground = p;
    *data.darkBackground = data._darkBackground;
}

void uiSetBoundingBoxPointer(int *p) {
    data.boundingBox = p;
    *data.boundingBox = data._boundingBox;
}

void uiSetSubdivisionsCallback(void (*callback)(int)) {
    data.subdivisions = callback;
}

void uiSetEraseFluidCallback(void (*callback)()) {
    data.eraseFluid = callback;
}

