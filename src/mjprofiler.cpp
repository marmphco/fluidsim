/*
    mjprofiler.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjprofiler.h"

namespace mcjee {

Profiler::Profiler() {}

void Profiler::addProfile(GLUI *glui, const std::string name) {
    profile p = {glui->add_statictext(name.c_str()), 0};
    profiles[name] = p;
}

void Profiler::start(const std::string name) {
    profiles[name].startTime = glutGet(GLUT_ELAPSED_TIME);
}

void Profiler::end(const std::string name) {
    long currentTime = glutGet(GLUT_ELAPSED_TIME);
    char computeTimeString[16];
    sprintf(computeTimeString, "%s: %ldms", name.c_str(), currentTime-profiles[name].startTime);
    profiles[name].control->set_text(computeTimeString);
    profiles[name].control->draw_text();
}

}
