/*
    mjprofiler.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjprofiler.h"

namespace mcjee {

Profiler::Profiler(GLUI *glui) : glui(glui), enabled(true) {
    panel = glui->add_panel("Profiler");
    GLUI_Control *control = glui->add_checkbox_to_panel(panel, "Enable Profiling", &enabled);
    control->set_int_val(1);
    glui->add_separator_to_panel(panel);
}

void Profiler::addProfile(const std::string name) {
    profile p = {glui->add_statictext_to_panel(panel, name.c_str()), 0};
    profiles[name] = p;
}

void Profiler::start(const std::string name) {
    profiles[name].startTime = glutGet(GLUT_ELAPSED_TIME);
}

void Profiler::end(const std::string name) {
    if (enabled) {
        glFinish();
        long currentTime = glutGet(GLUT_ELAPSED_TIME);
        char computeTimeString[16];
        sprintf(computeTimeString, "%s: %ldms", name.c_str(), currentTime-profiles[name].startTime);
        profiles[name].control->set_text(computeTimeString);
        profiles[name].control->draw_text();
    } else {
        char str[16];
        sprintf(str, "%s: N/A", name.c_str());
        profiles[name].control->set_text(str);
        profiles[name].control->draw_text();
    }
}

}
