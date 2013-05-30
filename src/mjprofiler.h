/*
    mjprofiler.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_PROFILER_H
#define MJ_PROFILER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wunused-comparison"
#include "libglui/glui.h"
#pragma clang diagnostic pop
#include <map>
#include <string>

namespace mcjee {

class Profiler {
private:
    struct profile {
        GLUI_StaticText *control;
        long startTime;
    };
    std::map<const std::string, profile> profiles;
    GLUI *glui;
    int enabled;
public:
    Profiler();
    void addProfile(const std::string name);
    void start(const std::string name);
    void end(const std::string name);
};

}

#endif
