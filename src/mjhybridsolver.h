/*
    mjhybridsolver.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_HYBRID_SOLVER_H
#define MJ_HYBRID_SOLVER_H

#include "mj.h"
#include "mjfluid.h"

namespace mcjee {

class HybridSolver : public FluidSolver {
public:
    HybridSolver(int width, int height, int depth);
};

}

#endif
